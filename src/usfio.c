#include "usfio.h"

void usf_printtxt(char **text, uint64_t len, FILE *stream) {
	/* Prints an array of strings of length len to stream stream */
	uint64_t i;

	for (i = 0; i < len; i++)
		fprintf(stream, "%s", text[i]);
}

char *usf_ftos(char *file, char *options, uint64_t *l) {
	/* Reads a file file with options options and returns contents
	 * as a single 0-terminated string, with length in l, NULL if an error occured */

	FILE *f = fopen(file, options);
	if (f == NULL) return NULL;

	if (fseek(f, 0, SEEK_END) != 0) {
		fclose(f);
		return NULL; /* Error while seeking end of file */
	}

	uint64_t length = ftell(f);
	if (length <= 0) {
		fclose(f);
		return NULL;
	}

	rewind(f); /* Go back to start */

	char *str = malloc(length + 1); /* Adjust for terminator */

	if (fread(str, 1, length, f) == 0) {
		fclose(f);
		free(str);
		return NULL;
	}

	str[length] = '\0'; /* Terminate */

	fclose(f);
	if (l != NULL) *l = length;

	return str;
}

char **usf_ftot(char *file, char *options, uint64_t *l) {
	/* Reads a file with given options and and returns a 2D array of pointers to lines (incl. \n */

	char *filestring;
	uint64_t fslen;

	if ((filestring = usf_ftos(file, options, &fslen)) == NULL)
		return NULL;

	uint64_t nlines;
	nlines = usf_scount(filestring, '\n');

	if (l) *l = nlines;

	char **txt, **t, *s, *fs;
	txt = malloc(sizeof(char *) * nlines);

	uint64_t linelen;
	for (fs = filestring, t = txt; nlines > 0; nlines--) {
		/* For every line, find its start, alloc, copy and keep pointer in txt */

		linelen = strchr(fs, '\n') - fs + 2; /* Space for all chars until \n, plus it and \0 */
		s = malloc(linelen);
		memcpy(s, fs, linelen);
		s[linelen - 1] = '\0'; /* Cap off */

		fs += linelen - 1; /* \0 isn't present in filestring */
		*t++ = s;
	}

	free(filestring);

	return txt;
}

void usf_freetxt(char **text, uint64_t nlines) {
	/* Free a text array */

	uint64_t i;

	for (i = 0; i < nlines; i++)
		free(text[i]);
	free(text);
}
