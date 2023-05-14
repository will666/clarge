#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>

char *shift_args(int *argc, char ***argv); // extracted from https://github.com/tsoding/img2term
const char *human_size(size_t bytes);      // extracted from https://gist.github.com/dgoguerra/7194777
void logger_start(char *log_file, bool initialize);
void logger_stop(void);

#endif
