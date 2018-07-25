/* Ablakatov Mikhai | Ablakatov.m@gmail.com | 2018 */
#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <assert.h>

#include "list.h"

struct ls_opts {
	int a : 1;
	int l : 1;
} opts = {};

static int strcmp_qsort_func(char **a, char **b) {
	return strcmp(*a, *b);
}

static void ls_format_access(const struct stat stat, char *rights)
{
	assert(rights);

	memset(rights, '-', 10);
	rights[10] = '\0';

	if (S_ISREG(stat.st_mode))
		rights[0] = '-';
	else if (S_ISDIR(stat.st_mode))
		rights[0] = 'd';
	else if (S_ISLNK(stat.st_mode))
		rights[0] = 'l';
	else 	rights[0] = '?';

	if (stat.st_mode & S_IRUSR)
		rights[1] = 'r';
	if (stat.st_mode & S_IWUSR)
		rights[2] = 'w';
	if (stat.st_mode & S_IXUSR)
		rights[3] = 'x';

	if (stat.st_mode & S_IRGRP)
		rights[4] = 'r';
	if (stat.st_mode & S_IWGRP)
		rights[5] = 'w';
	if (stat.st_mode & S_IXGRP)
		rights[6] = 'x';

	if (stat.st_mode & S_IROTH)
		rights[7] = 'r';
	if (stat.st_mode & S_IWOTH)
		rights[8] = 'w';
	if (stat.st_mode & S_IXOTH)
		rights[9] = 'x';
}

static int ls_handle_l_option(const char **ent_names, int ent_num,
					const char *path, struct stat *stats,
							char **output_strings)
{
	assert(ent_names);
	assert(path);
	assert(output_strings);
	assert(stats);

	int i;

	for (i = 0; i < ent_num; i++) {
		char *pathname = NULL;
		asprintf(&pathname, "%s/%s", path, ent_names[i]);

		char access_rights[11] = {};
		ls_format_access(stats[i], access_rights);

		int number_of_links = stats[i].st_nlink;
		int owner_id = stats[i].st_uid;
		int group_id = stats[i].st_gid;
		unsigned long size = stats[i].st_size;
		time_t time = stats[i].st_mtime;

		if (asprintf(&output_strings[i], "%s %d %d %d %lu %s %s",
			     access_rights, number_of_links, owner_id, group_id,
			     size, ctime(&time), ent_names[i]) == -1) {
			free(pathname);
			return -1;
		}
		
		free(pathname);	
	}
}

static char **ls_handle_options(const char **ent_names, int ent_num,
					const char *path, struct ls_opts opts)
{
	assert(ent_names);
	assert(path);

	int i;
	char **output_strings;
	struct stat *stats;

	output_strings = calloc(ent_num + 1, sizeof(char *));
	if (!output_strings)
		return NULL;
	stats = calloc(ent_num, sizeof(struct stat));
	if (!stats)
		return NULL;

	for (i = 0; i < ent_num; i++) {
		char *pathname = NULL;
		asprintf(&pathname, "%s/%s", path, ent_names[i]);
		if (!pathname) {
			free(output_strings);
			free(stats);
			return NULL;
		}

		if (stat(pathname, &stats[i]) == -1) {
			free(output_strings);
			free(stats);
			return NULL;
		}

		output_strings[i] = ent_names[i]; //by defualt
	}


	if (opts.l)
		ls_handle_l_option(ent_names, ent_num, path, stats, output_strings);
	else 

	free(stats);

	return output_strings;
}

int main(int argc, char* argv[])
{
	//init section
	int opt, dir_ind, i;
	//parse args
	while ((opt = getopt(argc, argv, "+l")) != -1) {
		switch (opt) {
		case 'l':
			opts.l = 1;
			break;
		default:
			fprintf(stderr, "Usage: %s [-l] directory\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if (optind < argc)
		dir_ind = optind;
	else {
		fprintf(stderr, "Usage: %s [-l] directory\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	//do the trick
	DIR *dir = opendir(argv[dir_ind]);
	if (!dir) {
		fprintf(stderr, "%s: can't open %s!\n",
					argv[0], argv[dir_ind]);
		exit(EXIT_FAILURE);
	}

	struct ls_dir_entry {
		char *rel_name;
		struct list_head node;
	}; //*dir_entries = calloc(1, sizeof(*dir_entries))
//	if (!last_dir_entry)
//		goto err;
//	init_list_head(&dir_entries->node);
	LIST_HEAD(dir_entries);

	struct dirent *curdirent = NULL;
	int entries_counter = 0;
	while (curdirent = readdir(dir)) {
		struct ls_dir_entry *entry = calloc(1, sizeof(*entry));
		if (!entry) {
			//TODO: free mem
			goto err;
		}
		if (asprintf(&entry->rel_name, "%s", curdirent->d_name) == -1) {
			//TODO: free mem
			goto err;
		}
		list_add_tail(&entry->node, &dir_entries);
		entries_counter++;
	}
	if (errno)
		goto err;

	char **entries_names = (char **)calloc(entries_counter + 1,
						sizeof(*entries_names));
	if (!entries_names) {
		//TODO: free
		goto err;
	}

	struct list_head *lh;
	for (lh = dir_entries.next, i = 0; lh != &dir_entries; lh = lh->next, i++)
		entries_names[i] = list_entry(lh, struct ls_dir_entry, node)->rel_name;
	qsort(entries_names, entries_counter, sizeof(char *), &strcmp_qsort_func);

	//TODO: handle -l option

	char **output_strings = ls_handle_options(entries_names,
					entries_counter, argv[dir_ind], opts);

	//TODO: print from a dictionary
	for (i = 0; i < entries_counter; i++)
		printf("%s\n", output_strings[i]);

	exit(EXIT_SUCCESS);

	//handle errors
err:
	perror(argv[0]);
	exit(EXIT_FAILURE);


	return 0;
}
