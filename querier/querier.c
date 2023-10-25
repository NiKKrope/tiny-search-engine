#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "../libcs50/webpage.h"
#include "../libcs50/bag.h"
#include "../libcs50/mem.h"
#include "../libcs50/file.h"
#include "../libcs50/hash.h"
#include "../libcs50/set.h"

#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "../libcs50/set.h"
#include "../indexer/index.c"

#define MAX_QUERY_SIZE 1024

// ! Run: ./querier ../common ../common/.indexer

// Cleans up the query
char* clean_query(char* query) {
    // Ignore blank lines and lines with only whitespace
    if (strspn(query, " \t\n") == strlen(query)) {
        return NULL;
    }

    // Convert to lowercase
    for (int i = 0; query[i]; i++) {
        query[i] = tolower(query[i]);
    }

    // Parse the query
    char* token = strtok(query, " \n");
    bool first_word = true;
    // Variable for keeping track of ANDs and ORs to make sure that they don't repeat
    bool prev_and_or = true; //? We assume that it's true because AND and OR can't be in the beginning
    char* fixed_query = mem_malloc(sizeof(query) * MAX_QUERY_SIZE);
    char* temp = mem_malloc(sizeof(query) * MAX_QUERY_SIZE); // We use this because sprintf cannot use the input sptring twice

    // Check for Syntax errors
    while (token != NULL) {
        if (strcmp(token, "and") == 0) {
            if (prev_and_or) return NULL; // Cancel if too many ANDs or ORs in a row
            temp = fixed_query;
            sprintf(fixed_query, "%s%s", temp, " AND ");
            prev_and_or = true;
        } else if (strcmp(token, "or") == 0) {
            if (prev_and_or) return NULL; // Cancel if too many ANDs or ORs in a row
            prev_and_or = true;
            temp = fixed_query;
            sprintf(fixed_query, "%s%s", temp, " OR ");
        } else if (!first_word) {
            if (prev_and_or == false) {
                temp = fixed_query;
                sprintf(fixed_query, "%s%s", temp, " AND ");
            }
            temp = fixed_query;
            sprintf(fixed_query, "%s%s", temp, token);
            prev_and_or = false;
        } else { // First word
            temp = fixed_query;
            sprintf(fixed_query, "%s%s", temp, token);
            first_word = false;
            prev_and_or = false;
        }
        token = strtok(NULL, " \n");
    }
    return fixed_query;
}

// Read the query from stdin
bool read_query(char* query, size_t size) {
    printf("Query: ");
    fflush(stdout);
    return fgets(query, size, stdin) != NULL;
}

// If the counter has a score for the AND sequences which is less than the previous word, we set that as the total score of the and sequence
void compare_min(void *arg, const int key, const int count) {
    arg = (counters_t*)(arg); // doc_min
    int value = counters_get(arg, key);
    if (value == 0) {
        return;
    }
    if (count < value) {
        counters_set(arg, key, count);
    }
}

// Set each counters value to 0
void reset_counter(void *arg, const int key, const int count) {
    arg = (counters_t*)(arg);
    counters_set(arg, key, 0);
}

// Set each counters value to the maximum value
void max_counter(void *arg, const int key, const int count) {
    arg = (counters_t*)(arg);
    counters_set(arg, key, __INT_MAX__);
}

// Add the minimum value of the AND sequense to the total score of the coresponsing docID
void add_ands(void *arg, const int key, const int count) {
    arg = (counters_t*)(arg); // doc_ranking
    if (count != __INT_MAX__) {
        counters_set(arg, key, counters_get(arg, key) + count); 
    }
}

// For each docID missing from the word found in the index (i.e. it doesn't contain the word), we set the words counter for that docID to 0
void check_if_contains(void* arg, const int key, const int count) {
    arg = (counters_t*)(arg);
    if (counters_get(arg, key) == 0) {
        counters_set(arg, key, 0);
    }
}

typedef struct search_result {
    int docID;
    int score;
    counters_t* printed_results;
} search_result_t;

void result_set_score(search_result_t* result, int new_docID, int new_score) {
    result->docID = new_docID;
    result->score = new_score;
}

int get_result_score(search_result_t* result) {
    return result->score;
}

counters_t* result_get_printed(search_result_t* result) {
    return result->printed_results;
}

void find_max_result(void* arg, const int key, const int count) {
    arg = (search_result_t*)(arg);
    if (count > get_result_score(arg)) {
        if (counters_get(result_get_printed(arg), key) == 0) {
            result_set_score(arg, key, count);
        }
    }
}

void print_results(counters_t* scores, char* pageDirectory) {
    // Create a scruct for the result of the search
    search_result_t* result = mem_malloc(sizeof(search_result_t));

    // Keep a list of printed counters
    counters_t* results_printed = counters_new();
    result->printed_results = results_printed;

    // Loop through and print all docID's from the result counters
    while (true) {
        result->score = 0; // Set a minimum score

        // Find the maximum score in the counter and save it to the result
        counters_iterate(scores, result, find_max_result);

        // If we have printed all results - break
        if (result->score == 0) {
            break;
        }

        char path[strlen(pageDirectory)+3];
        sprintf(path, "%s%s%d", pageDirectory, "/", result->docID);
        FILE* page_file = fopen(path, "r");
        if (page_file == NULL) {
            // Crawler webpage not found
            fprintf(stderr, "Search result page could not be found in %s\n", path);
            fclose(page_file);
            continue;
        }
        char* url = file_readLine(page_file);
        printf("Top Score %d;    DocID: %d;    Url:%s\n", result->score, result->docID, url);
        counters_set(result->printed_results, result->docID, __INT_MAX__);
    }
}







// Search for the given query
void search_query(char* query, index_t* index, char* page_dir) {
    // Count the number of documents (or webpages) that we have found in the crawler
    char* path = mem_malloc(sizeof(page_dir)+3);
    int doc_number = 1;
    sprintf(path, "%s%s%d", page_dir, "/", doc_number);
    FILE* crawler_file = fopen(path, "r");
    if (crawler_file != NULL) {
        doc_number += 1;
        sprintf(path, "%s%s%d", page_dir, "/", doc_number);
        crawler_file = fopen(path, "r");
    }

    // Counters for each docID and its Score
    counters_t* doc_ranking = counters_new();
    
    // Make a copy of the query
    char* query_copy = mem_malloc(sizeof(query));
    sprintf(query_copy, "%s", query);

    char* token = strtok(query_copy, " ");
    // Create counters with docID's that include the words from the query, which hold the score for each docID
    while (token != NULL) {
        if (strcmp(token, "AND") == 0) {
            token = strtok(NULL, " ");
            continue;
        }
        // For each word that we find inthe query, we add that to the counter and set its score to 0
        if (hashtable_find(index->index, token) == NULL) {
            token = strtok(NULL, " ");
            continue;
        }
        counters_iterate(hashtable_find(index->index, token), doc_ranking, reset_counter);
        token = strtok(NULL, " ");
    }

    // Now do the same things for another one with will hold the minimum values
    counters_t* doc_min = counters_new();
    
    // Reset query_copy to the original query
    free(query_copy);
    query_copy = mem_malloc(sizeof(query));
    sprintf(query_copy, "%s", query);

    token = strtok(query_copy, " ");
    // Create counters with docID's that include the words from the query, which hold the score for each docID
    while (token != NULL) {
        if (strcmp(token, "AND") == 0) {
            token = strtok(NULL, " ");
            continue;
        }
        // For each word that we find inthe query, we add that to the counter and set its score to 0
        counters_iterate(hashtable_find(index->index, token), doc_min, max_counter);
        token = strtok(NULL, " ");
    }

    // Reset query_copy to the original query
    free(query_copy);
    query_copy = mem_malloc(sizeof(query));
    sprintf(query_copy, "%s", query);

    // Add all OR sequences by calculating the minimum of the AND sequences
    token = strtok(query_copy, "OR");

    int i = 0;
    while (token != NULL) {
        char* word_in_sequence = strtok(token, " ");

        // Reset counters for min_values
        counters_iterate(hashtable_find(index->index, word_in_sequence), doc_min, max_counter);
        while (word_in_sequence != NULL) {
            if (strcmp(word_in_sequence, "AND") == 0) {
                word_in_sequence = strtok(NULL, " ");
                continue;
            }
            if (hashtable_find(index->index,word_in_sequence) == NULL) { 
                // Word not found in index, thus all docID's values of the word are reset to 0
                counters_iterate(doc_ranking, doc_min, reset_counter);
            }
            // Check if the docID contains the word, if not - then the docID's counter of the word in the index to 0
            counters_iterate(doc_min, hashtable_find(index->index,word_in_sequence), check_if_contains);
            // Set the value of an AND sequece from the query to the smallest value from the index
            counters_iterate(hashtable_find(index->index,word_in_sequence), doc_min, compare_min);
            word_in_sequence = strtok(NULL, " ");
        }
        counters_iterate(doc_min, doc_ranking, add_ands);
        
        // Reset query_copy string to the original query
        free(query_copy);
        query_copy = mem_malloc(sizeof(query));
        sprintf(query_copy, "%s", query);

        // Resstart the strtok for the original OR token
        token = strtok(query_copy, "OR");
        // Do it i amount of times, depending on how many ORs we find
        for (int j = 0; j <= i; j++) {
            token = strtok(NULL, "OR");
        }
        i++;
    }
    print_results(doc_ranking, page_dir);
}

int main(int argc, char* argv[]) {
    char* pageDirectory;
    char* indexFilename;
    
    // Parse Arguments
    if (argc != 3) {
        fprintf(stderr, "Please execute from a command line with usage syntax ./querier pageDirectory indexFilename \n");
        return 4;
    }

    // Directory for Crawler file
    pageDirectory = argv[1];
    // Check if ".crawler" file is in pageDirectoryectory
    char path[strlen(pageDirectory)+10];
    sprintf(path, "%s%s", pageDirectory, "/.crawler");
    FILE* crawler;
    crawler = fopen(path, "r");
    if (crawler == NULL) {
        // Crawler file not found
        fprintf(stderr, "No crawler output found \n");
        return 11;
    } else {
        fclose(crawler);
    }

    char path_1[strlen(pageDirectory)+3];
    sprintf(path_1, "%s%s", pageDirectory, "/1");
    crawler = fopen(path_1, "r");
    if (crawler == NULL) {
        // Crawler output ("pageDirectory/1") not found
        fprintf(stderr, "No crawler html output found \n");
        return 13;
    } else {
        fclose(crawler);
    }

    // Directory for indexFile
    indexFilename = argv[2];
    printf("%s\n", indexFilename);
    // Check if able to read the indexFile
    FILE* indexFile = fopen(indexFilename, "r");
    if (indexFile == NULL) {
        // indexFilename failed to read
        fprintf(stderr, "Unable to accesss %s \n", indexFilename);
        return 12;
    }
    
    index_t* index = index_load(indexFile); // Load the index and close the file
    
    if (index == NULL) {
        fprintf(stderr, "Unable to create index from %s \n", indexFilename);
        return 14;
    }
    
    // Read and search for input queries from the user
    char query[MAX_QUERY_SIZE];
    while (read_query(query, sizeof(query))) {
        char* input = query;
        char* new_query = query;
        input = clean_query(new_query);
        //free(new_query);
        if (input == NULL) {
            fprintf(stderr, "Invalid Query \n");
            return 15;
        }
        printf("Clean Query: %s\n", input);
        if (input != NULL) {
            search_query(input, index, pageDirectory);
        }
    }

    // Clean up
    index_delete(index);
    return 0;
}
