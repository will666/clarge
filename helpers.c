#include <stdio.h>
#include <assert.h>

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
    char length = sizeof(suffix) / sizeof(suffix[0]);

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
