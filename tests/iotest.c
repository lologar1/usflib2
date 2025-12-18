#include <stdio.h>
#include <inttypes.h>
#include "usfio.h"

char *textNoNewline[] =
	{ 	"First line\n",
		"Second line\n",
		"Third line no newline",
		"And followup\n",
		"We're done !\n", /* MANDATORY newline termination */
	};

char *textWellFormatted[] =
	{ 	"First line\n",
		"Second line\n",
		"Third line no newlineAnd followup\n",
		"We're done !\n",
	};

char **readout, *str;
uint64_t lines, i;

int main() {
	/* First write a file */
	FILE *toWrite = fopen("iotest_write.txt", "w");
	usf_printtxt(textNoNewline, 5, toWrite);
	fclose(toWrite);

	readout = usf_ftot("iotest_write.txt", &lines);
	for (i = 0; i < lines; i++) {
		if (strcmp(readout[i], textWellFormatted[i]) == 0)
			printf("iotest: Success comparison for line %"PRIu64": %s\n", i, readout[i]);
		else {
			printf("%s", readout[i]);
			printf("SEP\n");
			printf("%s", textWellFormatted[i]);
			printf("SEP\n");
		}
	}

	if ((str = usf_ftos("iotest_write.txt", &lines)) == NULL)
		printf("iotest: Error in usf_ftos while reading file\n");
	else
		printf("iotest: printing file in single string:\n%s", str);

	printf("iotest: binary file tests\n");
	char btest[] = {1, 2, 3, 4, 0, 5, 6};
	printf("Writing 1, 2, 3, 4, 0, 5, 6\n");
	usf_btof("binary.bin", btest, sizeof(btest));
	size_t b[1];
	char *gotback = usf_ftob("binary.bin", b);
	printf("Gotback %"PRId16" %"PRId16" %"PRId16" %"PRId16" %"PRId16" %"PRId16" %"PRId16" bytes %"PRIu64"\n", gotback[0], gotback[1], gotback[2], gotback[3], gotback[4], gotback[5], gotback[6], *b);

	printf("iotest: End of test !\n");
}
