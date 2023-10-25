#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"

int HASHTABLE_SIZE = 200;

typedef struct index {
    hashtable_t* index;
} index_t;

index_t* index_new() {
    index_t* new_index = mem_malloc(sizeof(index_t));
    hashtable_t* hashtable = hashtable_new(HASHTABLE_SIZE);
    if (hashtable == NULL) {
        return NULL;
    }

    new_index->index = hashtable;
    return new_index;
}
// Custom function
void deleteCounters(void* item) {
    counters_delete((counters_t*)item);
}

// Delete our hashtable and counters
void index_delete(index_t* index) {
    hashtable_delete(index->index, deleteCounters);
}

// Load the index from the indexer file
index_t* index_load(FILE* filename) {
    // Allocate a new index
    index_t* index = index_new();

    // Read each line from the file
    char* line = file_readLine(filename);
    char* word = strtok(line, " ");
    char* index_word = word;
    int docID, count;

    while (line != NULL) {
        // Parse the line into word and (docID, count) pairs
        if (word == NULL) {
            fprintf(stderr, "Error: incorrect index file\n");
            fclose(filename);
            return NULL;
        }

        counters_t* counter = counters_new();

        while (word != NULL) {
            // Get the next number separated by a " "
            word = strtok(NULL, " ");

            // Check because strtok sometimes gives (null)
            if (word == NULL) {
                break;
            }
            // Set it to the docID
            docID = atoi(word);
            
            // Get the one after that
            word = strtok(NULL, " ");
            // Set this one to the counter
            count = atoi(word);
            // Create a counter for it
            counters_set(counter, docID, count);
        }
        hashtable_insert(index->index, index_word, counter);
        line = file_readLine(filename);
        word = strtok(line, " ");
        index_word = word;
    }

    // Close the file and return the index
    fclose(filename);
    return index;
}
