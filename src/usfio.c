#include "usfio.h"

char *usf_ftos(char *file, u64 *l) {
	/* Reads a file with options "r" and returns the content as a single
	 * 0-terminated string of length l, or NULL if an error occurred. */

	FILE *f;
	if ((f = fopen(file, "r")) == NULL) return NULL; /* Failed to open */

	if (fseek(f, 0, SEEK_END) != 0) {
		fclose(f);
		return NULL; /* Error while seeking end of file */
	}

	u64 length;
	length = ftell(f);

	rewind(f); /* Set file position to start */

	char *str;
	str = usf_malloc(length + 1); /* Adjust for \0 terminator */

	if (fread(str, sizeof(char), length, f) != length) {
		fclose(f); /* Failed to read same number of bytes as file length */
		usf_free(str);
		return NULL;
	}

	str[length] = '\0'; /* Terminate */

	fclose(f);
	if (l) *l = length;

	return str;
}

char **usf_ftot(char *file, u64 *l) {
	/* Reads a file with options "r" and returns an array of pointers to its lines, excluding
	 * the \n character, or NULL if an error occurred. Each line is allocated separately.
	 * l is the number of lines present in the array.
	 * The file should contain at least one line (\n character).
	 * Note: this function should only be called when the underlying memory representation of the
	 * text array matters. Otherwise, usf_ftost should be called. */

	char *filestring;
	if ((filestring = usf_ftos(file, NULL)) == NULL) return NULL; /* usf_ftos failed */

	u64 nlines;
	nlines = usf_scount(filestring, '\n');

	if (l) *l = nlines;

	u64 linelen;
	char **text, **textptr, *line, *filestringptr;
	text = usf_malloc(sizeof(char *) * nlines);

	for (filestringptr = filestring, textptr = text; nlines > 0; nlines--) {
		linelen = strchr(filestringptr, '\n') - filestringptr + 1; /* Space for all chars plus \0 */
		line = usf_malloc(linelen);
		memcpy(line, filestringptr, linelen);
		line[linelen - 1] = '\0';

		filestringptr += linelen; /* \0 takes the place of \n so linelen is the correct offset */
		*textptr++ = line; /* Add to text array */
	}

	usf_free(filestring);

	return text;
}

char **usf_ftost(char *file, u64 *l) {
	/* Reads a file with options "r" and returns an array of pointers to its lines, excluding
	 * the \n character, or NULL if an error ocurred. Each line is not separately allocated.
	 * l is the number of lines present in the array.
	 * The file should contain at least one line (\n character). */

	char *filestring;
	if ((filestring = usf_ftos(file, NULL)) == NULL) return NULL; /* usf_ftos failed */

	char **stringtext;
	stringtext = usf_scsplit(filestring, '\n', l);
	--*l; /* omit last empty substring from usf_scsplit */

	return stringtext;
}

void *usf_ftob(char *file, u64 *size) {
	/* Reads a binary file with options "rb" and returns a pointer to its
	 * content or NULL if an error occurred.
	 * size is set to the size in bytes of the file. */

	FILE *f;
	if ((f = fopen(file, "rb")) == NULL) return NULL; /* Failed to open */

	u64 filesize; /* Query file size in bytes */
	fseek(f, 0, SEEK_END);
	filesize = ftell(f);
	fseek(f, 0, SEEK_SET);

	void *array;
	array = usf_malloc(filesize);
	if (fread(array, sizeof(char), filesize, f) != filesize) { /* Failed to read */
		usf_free(array);
		fclose(f);
		return NULL;
	}

	if (size) *size = filesize;
	fclose(f);

	return array;
}

u64 usf_btof(char *file, void *pointer, u64 size) {
	/* Writes size bytes from pointer to file with options "wb".
	 * Returns the number of bytes written, or 0 on error. */

	FILE *f;
	if ((f = fopen(file, "wb")) == NULL) return 0; /* Failed to open */

	u64 written;
	written = fwrite(pointer, sizeof(char), size, f);
	fclose(f);

	return written == size ? written : 0; /* 0 if failed to write */
}

void usf_printtxt(char **text, u64 len, FILE *stream) {
	/* Prints an array of strings of length len to stream stream */

	for (u64 i = 0; i < len; i++) fprintf(stream, "%s", text[i]);
}

void usf_freetxt(char **text, u64 nlines) {
	/* Frees an array of strings of size nlines. */

	u64 i;
	for (i = 0; i < nlines; i++)
		usf_free(text[i]);
	usf_free(text);
}
