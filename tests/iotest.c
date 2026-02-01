#include "usfio.h"

i32 main(void) {
	/* usfio.c test */

	u64 i;
	printf("iotest: Starting test!\n");

	const char *filecontents[] = {
		"firstline(newline)",
		"secondline(newline)",
		"thirdline(eof)"
	};

	/* usf_ftos, usf_ftost, usf_fprinttxt, usf_printtxt and usf_freetxt */
	u64 ftoslen;
	char **ftosfile;
	ftosfile = usf_ftost("iotest-file-read.txt", &ftoslen);

	if (ftosfile == NULL) {
		printf("iotest: Couldn't open (ftost/ftos) file \"iotest-file-read.txt\", aborting.\n");
		exit(1);
	} else printf("iotest: ftos OK\niotest: ftost OK\n");

	for (i = 0; i < ftoslen; i++) if (strcmp(ftosfile[i], filecontents[i])) {
		printf("iotest: File contents mismatch (ftost/ftos), got \"%s\" while expecting \"%s\", aborting.\n",
				ftosfile[i], filecontents[i]);
		exit(2);
	}

	FILE *writefile;
	writefile = fopen("iotest-file-write.txt", "w");
	usf_fprinttxt(writefile, ftosfile, ftoslen);
	fclose(writefile);

	usf_freetxt(ftosfile, 1);

	/* usf_ftot */
	u64 ftotlen;
	char **ftotfile;
	ftotfile = usf_ftot("iotest-file-write.txt", &ftotlen);

	if (ftotfile == NULL) {
		printf("iotest: Couldn't open (ftot) file \"iotest-file.txt\", aborting.\n");
		exit(3);
	} else printf("iotest: ftot OK\n");

	for (i = 0; i < ftotlen; i++) if (strcmp(ftotfile[i], filecontents[i])) {
		printf("iotest: File contents mismatch (ftot/fprinttxt), got \"%s\" while expecting \"%s\", aborting.\n",
				ftotfile[i], filecontents[i]);
		exit(4);
	}
	printf("iotest: fprinttxt/printtxt OK\n"); /* Written by */

	usf_freetxt(ftotfile, ftotlen);

	/* usf_ftob and usf_btof */
	u64 buffercontents[10] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34 };
	if (usf_btof("iotest-buffer.txt", buffercontents, sizeof(buffercontents)) != sizeof(buffercontents)) {
		printf("iotest: Buffer write failure, aborting.\n");
		exit(5);
	}

	u64 *ftobbuffer;
	ftobbuffer = usf_ftob("iotest-buffer.txt", NULL);

	for (i = 0; i < countof(buffercontents); i++) if (ftobbuffer[i] != buffercontents[i]) {
		printf("iotest: Buffer contents mismatch, got %"PRIu64" while expecting %"PRIu64", aborting.\n",
				ftobbuffer[i], buffercontents[i]);
		exit(6);
	}
	printf("iotest: btof OK\niotest: ftob OK\n");

	usf_free(ftobbuffer);

	printf("iotest: usfio OK (ALL TESTS PASSED)\n");
	return 0;
}
