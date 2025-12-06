#include "usfstring.h"

int usf_indstrcmp(const void *a, const void *b) {
	/* Wrapper for strcmp for qsort */

	char **x = (char **) a;
	char **y = (char **) b;

	if (*x == NULL) {
		if (*y == NULL)
			return 0;
		return -1;
	} else if (*y == NULL)
		return 1;

	return strcmp(* (char **) a, * (char **) b);
}

int usf_indstrlen(const void *a, const void *b) {
	/* Wrapper for strlen for qsort */

	char **x = (char **) a;
	char **y = (char **) b;

	if (*x == NULL) {
		if (*y == NULL)
			return 0;
		return -1;
	} else if (*y == NULL)
		return 1;

	return strlen(* (char **) a) - strlen(* (char **) b);
}

char *usf_sstartswith(char *base, char *prefix) {
	/* Test if a string starts with another and return substring, or null */

	if (base == NULL || prefix == NULL) return NULL;

	while (*prefix) //Up to either \0 char
		if (*prefix++ != *base++) //Compare
			return NULL;
	return base;
}

int usf_sendswith(char *base, char *suffix) {
	/* Test if a string ends with another */

	int offset;

	offset = strlen(base) - strlen(suffix);

	//Is bigger; doesn't endwith
	if (offset < 0) return 0;

	return !strcmp(base + offset, suffix);
}

uint64_t usf_scount(char *str, char c) {
	/* Count the number of occurences of char c in string str */

	uint64_t count = 0;

	while (*str) {
		if (*str == c)
			count++;
		str++;
	}

	return count;
}

int usf_txtcontainsline(char **array, uint64_t len, char *string) {
	/* Check if a text contains a certain line (string) */

	uint64_t i;

	if (string == NULL)
		return 0;

	for (i = 0; i < len; i++) {
		if (array[i] == NULL)
			continue;

		if (!strcmp(array[i], string))
			return 1;
	}

	return 0;
}

void usf_reversetxtlines(char **array, uint64_t len) {
	/* Reverse all lines in a text */

	uint64_t i = 0, j = len - 1;
	char *temp;

	while (i < j) {
		//Swap
		temp = array[j];
		array[j] = array[i];
		array[i] = temp;

		//Offset pointers
		j--;
		i++;
	}
}

int usf_sreplace(char *s, char template, char replacement) {
	/* Replace all occurences of char template with char replacement in a string
	 * and return the number of occurences matched */

    int n = 0;

    while((s = strchr(s, template)) != NULL) {
        *s++ = replacement;
        n++;
    }

    return n;
}

char **usf_scsplit(char *str, char sep, uint64_t *count) {
	/* Splits a string along a given char and returns an array of pointers to the
	 * different substrings, 0-terminated (including the string itself, which is
	 * the first substring). Doesn't allocate for substrings.
	 * Count is the number of substrings, so 1 more than the number of separators. */

	uint64_t nsubstrings;
	nsubstrings = usf_scount(str, sep) + 1;

	if (count) *count = nsubstrings;

	uint64_t i;
	char **substrings;

	substrings = malloc(nsubstrings * sizeof(char *));

	for (i = 0; i < nsubstrings; i++) {
		substrings[i] = str; /* Place substring */
		str = strchr(str, sep);
		if (str) *str++ = '\0'; /* Terminate and move to next substring */
	}

	return substrings;
}

int usf_strcat(char *destination, size_t size, uint32_t n, ...) {
	/* Concatenates multiple strings into destination, returning 1 if success else 0 */
	size_t catsize = 0;

	va_list sizecheck, args;
	va_start(args, n);
	va_copy(sizecheck, args);

	uint32_t i;
	for (i = 0; i < n; i++)	catsize += strlen(va_arg(sizecheck, char *));
	va_end(sizecheck);

	if (++catsize > size) return 0; /* Include 0 terminator */

	strcpy(destination, va_arg(args, char *));
	for (i = 1; i < n; i++) strcat(destination, va_arg(args, char *));
	va_end(args);

	return 1;
}

int usf_vstrcat(char *destination, size_t size, uint32_t n, va_list args) {
	/* Concatenates multiple strings into destination, returning 1 if success else 0 */
	size_t catsize = 0;

	va_list sizecheck;
	va_copy(sizecheck, args);

	uint32_t i;
	for (i = 0; i < n; i++)	catsize += strlen(va_arg(sizecheck, char *));
	va_end(sizecheck);

	if (++catsize > size) return 0; /* Include 0 terminator */

	strcpy(destination, va_arg(args, char *));
	for (i = 1; i < n; i++) strcat(destination, va_arg(args, char *));

	return 1;
}
