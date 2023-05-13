#ifndef FILE_ITEM_H
#define FILE_ITEM_H

#include <stdlib.h>

typedef struct File_item
{
    char *name;
    char *path;
    size_t size;
} File_item;

File_item new_file_item(char *name, char *path, size_t size);

#endif
