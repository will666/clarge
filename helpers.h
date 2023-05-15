#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>

#ifndef DA_APPEND
#define DA_APPEND
// Author: Alexey Kutepov // Dynamic array allocation & append via struct ----------------------------

// typedef struct Alloc
// {
//     size_t *items;
//     size_t count;
//     size_t capacity;
// } Alloc;
//
// eg.:
// Alloc data = {0};
// da_append(&data, item);

#define DA_INIT_CAP 100
#define da_append(da, item)                                                            \
    do                                                                                 \
    {                                                                                  \
        if ((da)->count >= (da)->capacity)                                             \
        {                                                                              \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity * 2;   \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items)); \
            assert((da)->items != NULL && "Not enough memory!");                       \
        }                                                                              \
                                                                                       \
        (da)->items[(da)->count++] = (item);                                           \
    } while (0)
// -------------------------------------------------------------------------------------
#endif

char *shift_args(int *argc, char ***argv); // extracted from https://github.com/tsoding/img2term
const char *human_size(size_t bytes);      // extracted from https://gist.github.com/dgoguerra/7194777
void logger_start(char *log_file, bool initialize);
void logger_stop(void);
char *log_time(void);

#endif
