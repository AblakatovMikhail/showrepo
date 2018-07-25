/* Ablakatov Mikhai | Ablakatov.m@gmail.com | 2018 */
#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <dirent.h>

#include "list.h"

struct ls_opts {
	int l : 1;
} opts = {};

static int strcmp_qsort_func(char **a, char **b) {
	return strcmp(*a, *b);
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


	//TODO: print from a dictionary
	for (i = 0; entries_names[i]; i++)
		printf("%s\n", entries_names[i]);

	exit(EXIT_SUCCESS);

	//handle errors
err:
	perror(argv[0]);
	exit(EXIT_FAILURE);


	return 0;
}
