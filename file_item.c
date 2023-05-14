#include <stdlib.h>
#include <string.h>

#include "file_item.h"

File_item *new_file_item(char *name, char *path, size_t size)
{
    File_item *new_file_item = malloc(sizeof(File_item));
    new_file_item->name = malloc(strlen(name) + 1);
    new_file_item->path = malloc(strlen(path) + 1);
    new_file_item->size = size;
    strcpy(new_file_item->name, name);
    strcpy(new_file_item->path, path);

    return new_file_item;
}
