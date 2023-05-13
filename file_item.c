#include <stdlib.h>

#include "file_item.h"

File_item new_file_item(char *name, char *path, size_t size)
{
    File_item file_item = {
        .name = name,
        .path = path,
        .size = size,
    };

    return file_item;
}
