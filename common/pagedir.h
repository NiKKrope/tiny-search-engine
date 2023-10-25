#ifndef PAGEDIR_H
#define PAGEDIR_H

#include "../libcs50/webpage.h"

/* Initializes the page directory */
bool pagedir_init(const char* pageDirectory);

/* Saves a webpage to the page directory */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

#endif // PAGEDIR_H
