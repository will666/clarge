#define _GNU_SOURCE // TODO: check for GNU licence limitations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <assert.h>

#include "clarge.h"
#include "helpers.h"

// #define BIG_FILE_SIZE (1024 * 1024) // overrides from main.h - testing...
// #define ITEMS_ALLOC 10000

static const size_t file_size_limit = BIG_FILE_SIZE;
static int items_alloc = ITEMS_ALLOC;
static int total_dirs_unprocessed = 0;
static int total_files_found = 0;
static int total_files_unprocessed = 0;

#define EXPORT_TO_FILE true

int main(int argc, char **argv)
{
	// assert(argc > 0);

	if (argc == 1)
	{
		printf("[error] no path specified, see help below\n\n");
		const char *program = shift_args(&argc, &argv);
		usage(program);
		return EXIT_FAILURE;
	}

	// TODO: implement flags arg

	char *dir = argv[1];
	printf("\nLooking for files in: %s%s%s\n\n", BLUE, dir, NORMAL_COLOR);

	logger_start(STDERR_LOG_FILE, true);

	File_item *data = (File_item *)calloc(items_alloc, sizeof(File_item));

	if (data == NULL)
	{
		printf("%s\n", ERR_HEAP_ALLOC_MSG);
		return EXIT_FAILURE;
	}

	get_files(dir, &data);

	printf("\nResults:\n");
	printf("  - found: %s%i file(s) bigger than %s%s\n", GREEN, total_files_found, human_size(file_size_limit), NORMAL_COLOR);
	printf("  - unreadable files:         %s%i%s\n", BLUE, total_files_unprocessed, NORMAL_COLOR);
	printf("  - unaccessible directories: %s%i%s\n\n", BLUE, total_dirs_unprocessed, NORMAL_COLOR);
	printf("Logs:\n");
	printf("  - files found:       %s%s%s\n", BLUE, RESULTS_TXT_FILE, NORMAL_COLOR);
	printf("  - warnings & errors: %s%s%s\n\n", BLUE, STDERR_LOG_FILE, NORMAL_COLOR);

	if (EXPORT_TO_FILE && total_files_found > 0)
		save_to_file(&data, RESULTS_TXT_FILE);

	for (int i = 0; i < total_files_found; ++i)
	{
		free(data[i].name);
		free(data[i].path);
	}
	free(data);

	logger_stop();

	return EXIT_SUCCESS;
}

static void resize_data_heap(File_item **data, const size_t prev_size, const size_t nb_items)
{
	size_t new_size = prev_size + nb_items;
	*data = (File_item *)realloc(*data, new_size * sizeof(File_item));
}

static void get_files(char *dir, File_item **data)
{
	assert(items_alloc > total_files_found);

	if (total_files_found >= items_alloc)
	{
		items_alloc *= 2;
		resize_data_heap(data, (size_t)items_alloc, (size_t)(items_alloc * 2));
	}

	if (data == NULL)
	{
		printf("%s\n", ERR_HEAP_ALLOC_MSG);
		free(data);
		exit(EXIT_FAILURE);
	}

	DIR *d = opendir(dir);
	if (d == NULL)
	{
		fprintf(stderr, "[warning] could not open directory: %s\n", dir);
		++total_dirs_unprocessed;
		return;
	}

	struct dirent *dp;
	while ((dp = readdir(d)) != NULL)
	{
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			if (strcmp(dir, "/") == 0)
				dir = "";

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

					File_item *f = new_file_item(dp->d_name, dir, size);
					(*data)[total_files_found] = *f;
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
		fprintf(stderr, "[warning] could not get size of: %s\n", filename);
		++total_files_unprocessed;
		return 0;
	}

	return (size_t)st.st_size;
}

static void save_to_file(File_item **data, const char *dst)
{
	assert(total_files_found > 0);

	FILE *f = fopen(dst, "w");

	for (int i = 0; i < total_files_found; ++i)
	{
		if ((*data)[i].name != 0)
			fprintf(f, "%s/%s => %s\n", (*data)[i].path, (*data)[i].name, human_size((*data)[i].size));
	}

	fclose(f);
}

static void usage(const char *program)
{
	printf("Usage: %s [OPTIONS...] [PATH]\n", program);
	printf("Options:\n");
	printf("    -o      save results to specified file\n");
	printf("    -v      print warning messages\n");
	printf("    -h      print this help and exit\n");
	printf("Example:\n");
	printf("    $ %s -o export.txt\n", program);
	printf("    Save results to file \"%s\"\n", RESULTS_TXT_FILE);
}
