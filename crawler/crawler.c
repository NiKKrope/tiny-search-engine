#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../common/pagedir.c"

#include "../libcs50/webpage.h"
#include "../libcs50/bag.h"
#include "../libcs50/mem.h"
#include "../libcs50/file.h"
#include "../libcs50/hash.h"

#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "../libcs50/set.h"


// ./crawler/crawler http://cs50tse.cs.dartmouth.edu/tse/letters/ ~/cs50-dev/tse-NiKKrope/common 7

static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth) {
    if (argc != 4) {
        fprintf(stderr, "Please execute from the command line with usage syntax ./crawler seedURL pageDirectory depth \n");
        return;
    }

    *seedURL = normalizeURL(argv[1]);
    if (!isInternalURL(*seedURL)) {
        fprintf(stderr, "seedURL must be an internal URL");
        return;
    }

    *pageDirectory = argv[2];
    if (!pagedir_init(argv[2])) {
        fprintf(stderr, "Error creating .crawler file \n");
        return;
    }

    *maxDepth = atoi(argv[3]);
    if (1 > *maxDepth || *maxDepth > 10) {
        fprintf(stderr, "maxDepth must be between 1 and 10");
        return;
    }
}

static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen) {
    int pos = 0;
    char* result;
    printf("%d Scanning: %s \n", webpage_getDepth(page), webpage_getURL(page));
    while ((result = webpage_getNextURL(page, &pos)) != NULL) {
        webpage_t* newPage = webpage_new(result, 1+webpage_getDepth(page), NULL);
        printf("%d Found: %s \n", webpage_getDepth(page), webpage_getURL(newPage));
        result = normalizeURL(result);
        char hash_key[4];
        sprintf(hash_key, "%d", (int)hash_jenkins(result, 200));
        if (isInternalURL(result) && hashtable_find(pagesSeen, hash_key) == NULL) {
            if (hashtable_insert(pagesSeen, hash_key, result)) {
                bag_insert(pagesToCrawl, newPage);
                printf("%d Added: %s \n", webpage_getDepth(page), webpage_getURL(newPage));
            }
        } else {
            if (!isInternalURL(result)) {
                printf("%d IgnExtrn: %s \n", webpage_getDepth(page), webpage_getURL(newPage));
            }
            else if (hashtable_find(pagesSeen, hash_key) != NULL) {
                printf("%d IgnDupl: %s \n", webpage_getDepth(page), webpage_getURL(newPage));
            }
        }
        pos++;
    }
    
}

static void crawl(char* seedURL, char* pageDirectory, const int maxDepth) {
    // Initialize Hashtable
    hashtable_t* pagesSeen = hashtable_new(200);
    char hash_key[3];
    sprintf(hash_key, "%d", (int)hash_jenkins(seedURL, 200));
    hashtable_insert(pagesSeen, hash_key, seedURL);

    // Initialize Bag
    bag_t* pageBag = bag_new();
    webpage_t* seedPage = webpage_new(seedURL, 0, NULL);
    bag_insert(pageBag, seedPage);

    webpage_t* page;
    int docID = 1;
    while ((page = bag_extract(pageBag)) != NULL) {
        printf("%d Fetched: %s \n", webpage_getDepth(page), webpage_getURL(page));
        if (webpage_fetch(page)) {
            // Save the Webpage to PageDirectory
            pagedir_save(page, pageDirectory, docID);
            // Might want to change to <= in the future idk
            if (webpage_getDepth(page) < maxDepth) {
                pageScan(page, pageBag, pagesSeen);
            }
        }
        docID++;
    }
}

int main(int argc, char *argv[])
{
    char* seedURL;
    char* pageDirectory;
    int maxDepth;
    
    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
    crawl(seedURL, pageDirectory, maxDepth);

    return 0;
}