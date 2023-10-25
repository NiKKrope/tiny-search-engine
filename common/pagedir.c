#include "../libcs50/file.h"
#include "../libcs50/webpage.h"
#include <string.h>

bool pagedir_init(const char* pageDirectory) {
    char path[strlen(pageDirectory)+10];
    sprintf(path, "%s%s", pageDirectory, "/.crawler");
    FILE* crawler;
    crawler = fopen(path, "w");

    if(crawler == NULL) {
        return false;
    }

    fclose(crawler);
    return true;
}

void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID) {
    // Fetch .crawler file
    char path[strlen(pageDirectory)+4];
    sprintf(path, "%s/%d", pageDirectory, docID);
    FILE* crawler;
    crawler = fopen(path, "w");

    // Fetch and print page contents
    char* html = webpage_getHTML(page);
    int depth = webpage_getDepth(page);
    char* url = webpage_getURL(page);
    fprintf(crawler, "%s\n", url);
    fprintf(crawler, "%d\n", depth);
    fprintf(crawler, "%s\n", html);

    // Close file and webpage
    fclose(crawler);
}