#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

// - Testing...
// #define LOG_PATH "./logs/"
// #define BIG_FILE_SIZE (1024 * 1024) // overrides from main.h - testing...
// #define ITEMS_ALLOC 10000
// -

#include "clarge.h"
#include "helpers.h"

static const size_t file_size_limit = TARGET_FILE_SIZE;
static int items_alloc = ITEMS_ALLOC;
static int total_dirs_unprocessed = 0;
static int total_files_found = 0;
static int total_files_unprocessed = 0;
static bool opt_verbose = false;
static bool opt_save_outpout = false;

int main(int argc, char **argv)
{
	double time_spent = 0.0;
	clock_t begin = clock();

	const char *program = shift_args(&argc, &argv);

	if (argc == 0)
	{
		printf("[error] %s - no path specified, see help below\n\n", log_time());
		usage(program);
		return EXIT_FAILURE;
	}

	while (argc > 0)
	{
		char *flag = shift_args(&argc, &argv);
		if (strcmp(flag, "-d") == 0)
		{
			if (argc <= 0)
			{
				printf("[error] %s - no path specified, see help below\n\n", log_time());
				return EXIT_FAILURE;
			}
			break;
		}
		else if (strcmp(flag, "-v") == 0)
		{
			opt_verbose = true;
		}
		else if (strcmp(flag, "-o") == 0)
		{
			opt_save_outpout = true;
		}
		else if (strcmp(flag, "-h") == 0)
		{
			usage(program);
			return EXIT_SUCCESS;
		}
		else
		{
			char *file_path = flag;

			printf("\nLooking for files in: %s%s%s\n\n", BLUE, file_path, NORMAL_COLOR);

			File_item *data = (File_item *)calloc(items_alloc, sizeof(File_item));
			if (data == NULL)
			{
				printf("%s\n", ERR_HEAP_ALLOC_MSG);
				return EXIT_FAILURE;
			}

			if (!opt_verbose)
				logger_start(STDERR_LOG_FILE, true);

			get_files(file_path, &data);

			if (opt_save_outpout && total_files_found > 0)
				save_to_file(&data, RESULTS_TXT_FILE);

			for (int i = 0; i < total_files_found; ++i)
			{
				free(data[i].name);
				free(data[i].path);
			}
			free(data);

			if (!opt_verbose)
				logger_stop();

			clock_t end = clock();
			time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

			printf("\nResults:\n");
			printf("  - found: %s%i file(s) bigger than %s%s\n", GREEN, total_files_found, human_size(file_size_limit), NORMAL_COLOR);
			printf("  - unreadable file(s):       %s%i%s\n", BLUE, total_files_unprocessed, NORMAL_COLOR);
			printf("  - unaccessible directories: %s%i%s\n", BLUE, total_dirs_unprocessed, NORMAL_COLOR);
			printf("  - execution time:           %.2fs\n", time_spent);
			if (!opt_verbose || opt_save_outpout)
				printf("\nLogs:\n");
			if (opt_save_outpout)
				printf("  - files found:       %s%s%s\n", GREEN, RESULTS_TXT_FILE, NORMAL_COLOR);
			if (!opt_verbose)
				printf("  - warnings & errors: %s%s%s\n", BLUE, STDERR_LOG_FILE, NORMAL_COLOR);
			printf("\n");
		}
	}

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
	{
		if (d == NULL)
		{
			char msg[] = "[warning] %s - could not open directory: %s\n";
			if (opt_verbose)
				printf(msg, log_time(), dir);
			else
				fprintf(stderr, msg, log_time(), dir);
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

				char p[500];
				snprintf(p, sizeof(p), "%s/%s", dir, dp->d_name);

				if (dp->d_type == 4)
					get_files(p, data);
				else
				{
					size_t size = getFilesize(p);

					if (size != 0 && size >= file_size_limit)
					{
						printf("%s%s/%s => %s%s\n", GREEN, dir, dp->d_name, human_size(size), NORMAL_COLOR);
						if (!opt_verbose)
							fprintf(stderr, "[INFO] %s - %s/%s => %s\n", log_time(), dir, dp->d_name, human_size(size));

						File_item *f = new_file_item(dp->d_name, dir, size);
						(*data)[total_files_found] = *f;
						++total_files_found;
					}
				}
			}
		}
	}
	closedir(d);
}

static size_t getFilesize(const char *filename)
{
	struct stat st;

	if (stat(filename, &st) < 0)
	{
		char msg[] = "[warning] %s - could not get size of: %s\n";
		if (opt_verbose)
			printf(msg, log_time(), filename);
		else
			fprintf(stderr, msg, log_time(), filename);
		++total_files_unprocessed;
		return 0;
	}

	return (size_t)st.st_size;
}

static void save_to_file(File_item **data, const char *dst)
{
	assert(total_files_found > 0);

	FILE *f = fopen(dst, "w");
	{
		for (int i = 0; i < total_files_found; ++i)
		{
			if ((*data)[i].name != 0)
				fprintf(f, "%s/%s => %s\n", (*data)[i].path, (*data)[i].name, human_size((*data)[i].size));
		}
	}
	fclose(f);
}

static void usage(const char *program)
{
	printf("\nUsage: %s [OPTIONS...] <PATH>\n", program);
	printf("\nAnalyse recursively given directory and print out files of given size\n");
	printf("\nOptions:\n");
	printf("    -o      save results to specified file\n");
	printf("    -v      print warning messages instead of logging to file\n");
	printf("    -h      print this help and exit\n");
	printf("\nExample:\n");
	printf("    $ %s -o /var\n", program);
	printf("    Analyse \"/var\" and save results to \"%s\"\n\n", RESULTS_TXT_FILE);
}
