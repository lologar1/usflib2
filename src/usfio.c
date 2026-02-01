#include "usfio.h"

char *usf_ftos(const char *file, u64 *l) {
	/* Reads a file with options "r" and returns the content as a single
	 * 0-terminated string of length l, or NULL if an error occurred. */

	FILE *f;
	if ((f = fopen(file, "r")) == NULL) return NULL; /* Failed to open */

	if (fseek(f, 0, SEEK_END)) {
		fclose(f);
		return NULL; /* Error while seeking end of file */
	}

	u64 length;
	if ((length = (u64) ftell(f)) == U64_MAX) { /* ftell failed (-1) */
		fclose(f);
		return NULL;
	}

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

char **usf_ftot(const char *file, u64 *l) {
	/* Reads a file with options "r" and returns an array of pointers to its lines, excluding
	 * the \n character, or NULL if an error occurred. Each line is allocated separately.
	 * l is the number of lines present in the array.
	 * The file should contain at least one line (\n character), otherwise the return value is malloc(0).
	 * Note: this function should only be called when the underlying memory representation of the
	 * text array matters. Otherwise, usf_ftost should be called. */

	char *filestring;
	if ((filestring = usf_ftos(file, NULL)) == NULL) return NULL; /* usf_ftos failed */

	u64 nlines;
	nlines = usf_scount(filestring, '\n');

	if (l) *l = nlines;

	u64 linelen, nline;
	char **text, **textptr, *line, *filestringptr;
	text = usf_malloc(sizeof(char *) * nlines);

	for (filestringptr = filestring, textptr = text, nline = 0; nline < nlines; nline++) {
		char *newline;
		if ((newline = strchr(filestringptr, '\n')) == NULL) { /* Malformed text file */
			usf_freetxt(text, nline); /* Free all allocated lines */
			usf_free(filestring);
			return NULL;
		}
		linelen = (u64) (newline - filestringptr + 1); /* Fit all chars plus \0 */

		line = usf_malloc(linelen);
		memcpy(line, filestringptr, linelen);
		line[linelen - 1] = '\0';

		filestringptr += linelen; /* \0 takes the place of \n so linelen is the correct offset */
		*textptr++ = line; /* Add to text array */
	}

	usf_free(filestring);
	return text;
}

char **usf_ftost(const char *file, u64 *l) {
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

void *usf_ftob(const char *file, u64 *size) {
	/* Reads a binary file with options "rb" and returns a pointer to its
	 * content or NULL if an error occurred.
	 * size is set to the size in bytes of the file. */

	FILE *f;
	if ((f = fopen(file, "rb")) == NULL) return NULL; /* Failed to open */

	u64 filesize; /* Query file size in bytes */
	fseek(f, 0, SEEK_END);
	if ((filesize = (u64) ftell(f)) == U64_MAX) { /* ftell failed (-1) */
		fclose(f);
		return NULL;
	}
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

u64 usf_btof(const char *file, const void *pointer, u64 size) {
	/* Writes size bytes from pointer to file with options "wb".
	 * Returns the number of bytes written, or 0 on error. */

	FILE *f;
	if ((f = fopen(file, "wb")) == NULL) return 0; /* Failed to open */

	u64 written;
	written = fwrite(pointer, sizeof(char), size, f);
	fclose(f);

	return written == size ? written : 0; /* 0 if failed to write */
}

void usf_fprinttxt(FILE *stream, char *const *text, u64 len) { /* __REVISE__ cstyle@1.8.1 */
	/* Prints an array of strings of length len to specified stream, newline-separated */

	for (u64 i = 0; i < len; i++) fprintf(stream, "%s\n", text[i]);
}

void usf_printtxt(char *const *text, u64 len) { /* __REVISE__ cstyle@1.8.1 */
	/* stdout wrapper for usf_fprinttxt */

	usf_fprinttxt(stdout, text, len);
}

void usf_freetxt(char **text, u64 nlines) {
	/* Frees an array of strings of size nlines. */

	u64 i;
	for (i = 0; i < nlines; i++)
		usf_free(text[i]);
	usf_free(text);
}
