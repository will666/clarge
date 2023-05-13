#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <assert.h>

// #define BIG_FILE_SIZE (1024 * 1024) // overrides from main.h - testing...
// #define ITEMS_ALLOC 1000
#include "clarge.h"
#include "helpers.h"

static const size_t file_size_limit = BIG_FILE_SIZE;
static int items_alloc = ITEMS_ALLOC;
static int total_dirs_unprocessed = 0;
static int total_files_found = 0;
static int total_files_unprocessed = 0;

#define EXPORT_TO_FILE false

int main(int argc, char **argv)
{
	// assert(argc > 0);

	if (argc == 1)
	{
		fprintf(stderr, "Error: no path specified, see help below\n\n");
		const char *program = shift_args(&argc, &argv);
		usage(program);
		return EXIT_FAILURE;
	}

	// TODO: implement flags arg

	char *dir = argv[1];
	printf("Reading from: %s\n", dir);

	File_item *data = (File_item *)calloc(items_alloc, sizeof(File_item));

	if (data == NULL)
	{
		fprintf(stderr, "%s\n", ERR_HEAP_ALLOC_MSG);
		return EXIT_FAILURE;
	}

	get_files(dir, &data);

	fprintf(stdout, "\nResults:\n");
	fprintf(stdout, "  - found: %s%i file(s) bigger than %zu bytes%s\n", GREEN, total_files_found, file_size_limit, NORMAL_COLOR);
	fprintf(stdout, "  - unreadable files: %s%i%s\n", BLUE, total_files_unprocessed, NORMAL_COLOR);
	fprintf(stdout, "  - unaccessible directories: %s%i%s\n\n", BLUE, total_dirs_unprocessed, NORMAL_COLOR);

	// TODO: fix string encoding
	if (EXPORT_TO_FILE && total_files_found > 0)
		save_to_file(&data, "xxx.txt");

	free(data);
	return EXIT_SUCCESS;
}

static void resize_data_heap(File_item **data, const size_t prev_size, const size_t nb_items)
{
	size_t new_size = prev_size + nb_items;
	*data = (File_item *)realloc(*data, new_size * sizeof(File_item));
}

static void get_files(char *dir, File_item **data)
{
	if (total_files_found >= items_alloc)
	{
		resize_data_heap(data, (size_t)items_alloc, (size_t)(items_alloc * 2));
		items_alloc *= 2;
	}

	if (data == NULL)
	{
		fprintf(stderr, "%s\n", ERR_HEAP_ALLOC_MSG);
		free(data);
		exit(EXIT_FAILURE);
	}

	assert(items_alloc > total_files_found);

	DIR *d = opendir(dir);
	if (d == NULL)
	{
		printf("warning: could not open directory %s\n", dir);
		++total_dirs_unprocessed;
		return;
	}

	struct dirent *dp;
	while ((dp = readdir(d)) != NULL)
	{
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			if (strcmp(dir, "/") == 0)
			{
				dir = "";
			}

			char *p;
			asprintf(&p, "%s/%s", dir, dp->d_name);

			if (dp->d_type == DT_DIR)
			{
				get_files(p, data);
			}
			else
			{
				size_t size = getFilesize(p);

				if (size != 0 && size >= file_size_limit)
				{
					printf("%s%s/%s => %s%s\n", GREEN, dir, dp->d_name, human_size(size), NORMAL_COLOR);

					File_item f = new_file_item(dp->d_name, dir, size);
					(*data)[total_files_found] = f;

					++total_files_found;
				}
			}

			free(p);
		}
	}
	closedir(d);
}

static size_t getFilesize(const char *filename)
{
	struct stat st;
	if (stat(filename, &st) < 0)
	{
		printf("could not get size of: %s\n", filename);
		++total_files_unprocessed;
		return 0;
	}
	return (size_t)st.st_size;
}

static void save_to_file(File_item **data, const char *dst)
{
	FILE *f = fopen(dst, "w+");

	for (int i = 0; i < total_files_found; ++i)
	{
		if ((*data)[i].name != 0)
		{
			fprintf(f, "%s/%s => %lu\n", (*data)[i].path, (*data)[i].name, (*data)[i].size);
		}
		else
		{
			break;
		}
	}

	fclose(f);
}

static void usage(const char *program)
{
	fprintf(stderr, "Usage: %s [OPTIONS...] [PATH]\n", program);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "    -o      save results to specified file\n");
	fprintf(stderr, "    -v      print warning messages\n");
	fprintf(stderr, "    -h      print this help and exit\n");
	fprintf(stderr, "Example:\n");
	fprintf(stderr, "    $ %s -o export.txt\n", program);
	fprintf(stderr, "    Save results to file \"export.txt\"\n");
}
