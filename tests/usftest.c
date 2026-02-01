#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "usfstd.h"
#include "usfaesc.h"

i32 main(i32 nargs, char *argv[]) {
	/* Test the usflib2 library */

	if (nargs < 1) {
		printf("usftest: nargs is 0 (invalid state), aborting.\n");
		exit(1);
	}
	char *programname, *separator;
	for (programname = separator = argv[0]; (separator = strchr(separator, '/')); programname = ++separator);

	printf("usftest: File extension "AESC_COLOR_SET_BOLD".usftest"AESC_RESET_ALL
			" is used for labeling usflib2 test files.\n");
	printf("usftest: "AESC_COLOR_FG_YELLOW"Starting all tests!"AESC_RESET_ALL"\n");

	DIR *directory;
	if ((directory = opendir(".")) == NULL) {
		printf("usftest: Couldn't open current directory, aborting!\n");
		exit(2);
	}

	struct dirent *entry;
	while ((entry = readdir(directory))) {
		char *name;
		if ((separator = strchr(name = entry->d_name, '.')) == NULL) continue; /* Not a candidate */
		if (strcmp(separator, ".usftest")) continue; /* Not a candidate */
		if (!strcmp(name, programname)) continue; /* Don't execute yourself */

		char testcommand[512] = {'.', '/'};
		if (strlen(name) + 1 > sizeof(testcommand)) {
			printf("usftest: test %s surpasses name limit %"PRIu64", skipping.\n",
					name, (u64) sizeof(testcommand));
			continue;
		}
		strcpy(testcommand + 2, name);

		i32 r;
		if ((r = system(testcommand))) {
			printf("usftest: test %s returned non-zero exit value %"PRId32", aborting.\n",
					name, r);
			exit(3);
		}
	}
	closedir(directory);

	printf("usftest: "AESC_COLOR_FG_BRIGHT_GREEN"usftest OK (ALL TESTS PASSED)"AESC_RESET_ALL"\n");

	return 0;
}
