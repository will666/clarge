#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

#include "helpers.h"

char *shift_args(int *argc, char ***argv)
{
    assert(*argc > 0);
    char *result = **argv;
    *argc -= 1;
    *argv += 1;
    return result;
}

const char *human_size(size_t bytes)
{
    char *suffix[] = {"B", "KiB", "MiB", "GiB", "TiB"};
    int length = sizeof(suffix) / sizeof(suffix[0]);

    int i = 0;
    double dblBytes = bytes;

    if (bytes > 1024)
    {
        for (i = 0; (bytes / 1024) > 0 && i < length - 1; i++, bytes /= 1024)
            dblBytes = bytes / 1024.0;
    }

    static char output[50];
    sprintf(output, "%.02lf %s", dblBytes, suffix[i]);

    return output;
}

void logger_start(char *log_file, bool initialize)
{
    if (initialize)
    {
        FILE *f = fopen(log_file, "w");
        fclose(f);
    }

    int stderr_log_file = open(log_file, O_RDWR | O_CREAT | O_APPEND, 0600);
    dup2(stderr_log_file, STDERR_FILENO);
    close(stderr_log_file);
}

void logger_stop(void)
{
    fflush(stderr);
    close(STDERR_FILENO);
}
