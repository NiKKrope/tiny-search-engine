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

#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "../libcs50/set.h"
#include "index.c"

// Bash commands for testing:
// ~/cs50-dev/tse-NiKKrope/crawler/crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ~/cs50-dev/tse-NiKKrope/common 7
// ~/cs50-dev/tse-NiKKrope/indexer ~/cs50-dev/tse-NiKKrope/common ~/cs50-dev/tse-NiKKrope/common/indexer_file

void indexPage(FILE* page, index_t* index, const int docID);
void indexBuild(char *pageDirectory, char *indexFile);
void printIndexLine(void* arg, const char* key, void* item);
void printIndex(index_t* index, char* indexFile);

int main(int argc, char *argv[])
{
    char* pageDirectory;
    char* indexFilename;
    
    // Parse Arguments
    if (argc != 3) {
        fprintf(stderr, "Please execute from a command line with usage syntax ./indexer pageDirectory indexFilename \n");
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

    // Output into indexFile
    indexFilename = argv[2];
    // Check if able to create/modify indexFile
    FILE* indexFile;
    indexFile = fopen(indexFilename, "w");
    if (indexFile == NULL) {
        // indexFilename failed to read
        fprintf(stderr, "Unable to accesss %s \n", indexFilename);
        return 12;
    } else {
        fclose(indexFile);
    }

    // Initialize the index and scan all the page files
    indexBuild(pageDirectory, indexFilename);

    return 0;
}

void indexBuild(char *pageDirectory, char *indexFile) {
    // Create index
    index_t *index = index_new();

    // Loop over document IDs, starting from 1
    int docID = 1;
    while (true) {
        // Load webpage file
        char* path = malloc(strlen(pageDirectory) + 10); // allocate enough space for "/id" and null terminator
        sprintf(path, "%s/%d", pageDirectory, docID);
        FILE* page = fopen(path, "r");
        free(path);
        
        // If file is loaded successfully, index it
        if (page != NULL) {
            indexPage(page, index, docID);
        } else {
            break;
        }
        fclose(page);
        docID++;
    }

    // Print our index to our designated file
    printIndex(index, indexFile);

    // Clean up
    index_delete(index);
}

void indexPage(FILE* page, index_t* index, const int docID) {
    int pos = 0;
    char* word = NULL;
    char* url = NULL;
    url = file_readLine(page); // Read the url of the page from the text file created by the crawler
    word = file_readLine(page); // Read the docID from the file created by the crawler

    webpage_t* webpage = webpage_new(url, docID, NULL); // Create a webpage stuct with the info we read
    webpage_fetch(webpage); // Get the page's HTML

    // Go over each word in the page and add it to the index
    while ((word != NULL)) {

        word = webpage_getNextWord(webpage, &pos);

        // Stop if nothing (had a problem with this before, idk why this happends but the check is useful nonetheless)
        if (word == NULL) {
            break;
        }

        // Skip if word is too short
        if (strlen(word) < 3 || word == NULL) {
            continue;
        }
        
        // Normalize the word
        for (int i = 0; i < strlen(word); i++) {
            word[i] = tolower(word[i]);
        }
        
        // Look up the word in the index
        counters_t *counters = hashtable_find(index->index, word);
        if (counters == NULL) {
            // First time seeing a word
            counters = counters_new();
            hashtable_insert(index->index, word, counters);
        }

        // Update the counters for this webpage
        counters_add(counters, docID);
    }
}

// ### 
// Print Functions
// ###

// Print a counters object
void printCounters(void *arg, const int key, const int count) {
    FILE* indexFile = (FILE*) arg;
    fprintf(indexFile, "%d %d ", key, count);
}

// Print the things from the set and counter in the hashtable
void printIndexLine(void* arg, const char* key, void* item) {
    FILE* indexFile = (FILE*) arg;
    counters_t* counter = (counters_t* ) item;
    fprintf(indexFile, "%s ", key);
    counters_iterate(counter, indexFile, printCounters);
    fprintf(indexFile, "\n");
}

// Print the info from the index
void printIndex(index_t* index, char* indexFile) {
    // Write index to file
    FILE* index_file = fopen(indexFile, "w");
    if (index_file == NULL) {
        fprintf(stderr, "Failed to open indexFile");
        return;
    } else {
        hashtable_iterate(index->index, index_file, printIndexLine);
    }
}