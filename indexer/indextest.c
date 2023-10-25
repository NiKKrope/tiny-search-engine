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

void printIndexLine(void* arg, const char* key, void* item);
void printIndex(index_t* index, char* indexFile);

int main(int argc, char *argv[]) {
    // Ensure correct usage
    if (argc != 3) {
        fprintf(stderr, "Usage: ./indextest oldIndexFilename newIndexFilename\n");
        return 1;
    }

    char *oldIndexFilename = argv[1];
    char *newIndexFilename = argv[2];

    // Open old index file for reading
    FILE *oldIndexFile = fopen(oldIndexFilename, "r");
    if (oldIndexFile == NULL) {
        fprintf(stderr, "Error: could not open file '%s' for reading\n", oldIndexFilename);
        return 2;
    }

    // Load index from IndexFile
    index_t* index = index_load(oldIndexFile);

    // Write index to file (same function as indexer)
    printIndex(index, newIndexFilename);
    
    // Clean up index
    index_delete(index);

    return 0;
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