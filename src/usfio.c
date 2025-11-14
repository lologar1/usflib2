#include "usfio.h"

void usf_printtxt(char **text, uint64_t len, FILE *stream) {
	/* Prints an array of strings of length len to stream stream */
	uint64_t i;

	for (i = 0; i < len; i++)
		fprintf(stream, "%s", text[i]);
}

char *usf_ftos(char *file, uint64_t *l) {
	/* Reads a file file and returns contents as a single 0-terminated string,
	 * with length in l, NULL if an error occured */

	FILE *f = fopen(file, "r");
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

char **usf_ftot(char *file, uint64_t *l) {
	/* Reads a file and returns a 2D array of pointers to lines (incl. \n */

	char *filestring;
	uint64_t fslen;

	if ((filestring = usf_ftos(file, &fslen)) == NULL)
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

size_t usf_btof(char *file, void *pointer, size_t size) {
	/* Dumps size bytes from pointer into binary file file, returning the number of bytes successfully written */
	FILE *f;
	size_t written;

	f = fopen(file, "wb");
	written = fwrite(pointer, 1, size, f);
	fclose(f);

	return written;
}

void *usf_ftob(char *file, size_t *size) {
	/* Reads a binary file and returns a pointer to its data, with size (in bytes) written to size */
	FILE *f;
	void *array;
	size_t sz;

	if ((f = fopen(file, "rb")) == NULL) return NULL;

	/* Query file size in bytes */
	fseek(f, 0, SEEK_END);
	sz = ftell(f);
	fseek(f, 0, SEEK_SET);

	array = malloc(sz);
	if (fread(array, 1, sz, f) != sz) {
		/* Did not read as many bytes as size : error occured */
		free(array);
		fclose(f);
		return NULL;
	}
	*size = sz;
	fclose(f);

	return array;
}
