#ifndef CLARGE_H
#define CLARGE_H

#include <stdlib.h>

#include "file_item.h"

#ifndef HELPERS_H
#ifndef LOG_PATH
#define LOG_PATH "/tmp/"
#endif
#ifndef STDERR_LOG_FILE
#define STDERR_LOG_FILE LOG_PATH "clarge.log"
#endif
#endif

#ifndef RESULTS_TXT_FILE
#define RESULTS_TXT_FILE "clarge_results.txt"
#endif

#define NORMAL_COLOR "\x1B[0m"
#define GREEN "\x1B[32m"
#define BLUE "\x1B[34m"

#ifndef ITEMS_ALLOC
#define ITEMS_ALLOC 100
#endif

#ifndef TARGET_FILE_SIZE
#define TARGET_FILE_SIZE (1024 * 1024 * 1024)
#endif

#define ERR_HEAP_ALLOC_MSG "[error] could not allocate heap memory"

static void get_files(char *file_path, File_item **data);
static void resize_data_heap(File_item **data, const size_t prev_size, const size_t nb_items);
static size_t getFilesize(const char *filename);
static void save_to_file(File_item **data, const char *dst);
static void usage(const char *program);

#endif
