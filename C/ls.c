/* Ablakatov Mikhai | Ablakatov.m@gmail.com | 2018 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <dirent.h>

struct ls_opts {
	int l : 1;
} opts = {};

int main(int argc, char* argv[])
{
	//init section
	int opt, dir_ind;
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

	struct dirent *curdirent = NULL;
	while (curdirent = readdir(dir)) {
		printf("%s\n", curdirent->d_name); //TODO: add to a dictionary instead
		//TODO: handle -l option
	}
	if (errno) {
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}

	//TODO: print from a dictionary

	//handle errors
	return 0;
}
