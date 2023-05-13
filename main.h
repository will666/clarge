#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>

#include "file_item.h"

#define NORMAL_COLOR "\x1B[0m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"

#ifndef ITEMS_ALLOC
#define ITEMS_ALLOC 100
#endif

#ifndef BIG_FILE_SIZE
#define BIG_FILE_SIZE (1024 * 1024 * 1024)
#endif

#define ERR_HEAP_ALLOC_MSG "error: could not allocate heap memory"

static void get_files(char *dir, File_item **data);
static void resize_data_heap(File_item **data, const size_t prev_size, const size_t nb_items);
static size_t getFilesize(const char *filename);
static void save_to_file(File_item **data, const char *dst);
static void usage(const char *program);

#endif
