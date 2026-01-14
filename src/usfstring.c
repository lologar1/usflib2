#include "usfstring.h"

int usf_indstrcmp(const void *a, const void *b) {
	/* strcmp wrapper for qsort */

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
	/* strlen wrapper for qsort */

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
	/* Returns a pointer to the first char in base after the prefix (excluding terminating \0),
	 * or NULL if base does not start with prefix. */

	if (base == NULL || prefix == NULL) return NULL;

	while (*prefix) /* If a char in prefix mismatches, condition failed */
		if (*prefix++ != *base++) return NULL;

	return base;
}

char *usf_sendswith(char *base, char *suffix) {
	/* Returns a pointer to the first char in base before the suffix,
	 * or NULL if base does not end with suffix. */

	i64 offset;
	if ((offset = strlen(base) - strlen(suffix)) < 0) return NULL;

	char *start;
	start = base + offset;

	return strcmp(start, suffix) ? NULL : start;
}

u64 usf_scount(char *str, char c) {
	/* Returns the number of occurrences of char c in string str. */

	u64 count;
	for (count = 0; *str; str++)
		if (*str == c) count++;

	return count;
}

i32 usf_txtcontainsline(char **array, u64 len, char *string) {
	/* Returns 1 if the string array of length len contains string string, otherwise 0 */

	if (string == NULL) return 0;

	u64 i;
	for (i = 0; i < len; i++) {
		if (array[i] == NULL) continue;
		if (!strcmp(array[i], string)) return 1; /* Found match */
	}

	return 0;
}

void usf_reversetxtlines(char **array, u64 len) {
	/* Reverses the order of all the lines in the string array of length len. */

	u64 i, j;
	for (i = 0, j = len - 1; i < j; i++, j--)
		USF_SWAP(array[i], array[j]);
}

u64 usf_sreplace(char *str, char template, char replacement) {
	/* Replaces all occurrences of char template with char replacement in string str
	 * and return the number of successful substitutions */

    u64 substitutions;
	for (substitutions = 0; (str = strchr(str, template)) != NULL; substitutions++)
        *str++ = replacement;

    return substitutions;
}

char **usf_scsplit(char *str, char sep, u64 *count) {
	/* Splits the given string str on each character sep matched and returns an array of
	 * pointers to the resulting 0-terminated substringns.
	 * count is set to the number of substrings (1 more than the number of matched separators).
	 * No allocations are performed for the substrings. */

	u64 nsubstrings;
	nsubstrings = usf_scount(str, sep) + 1;

	if (count) *count = nsubstrings;

	char **substrings;
	substrings = usf_malloc(nsubstrings * sizeof(char *));

	u64 i;
	for (i = 0; i < nsubstrings; i++) {
		substrings[i] = str; /* Place substring */
		if ((str = strchr(str, sep))) *str++ = '\0'; /* Terminate if next separator exists */
	}

	return substrings;
}

i32 usf_strcat(char *destination, u64 size, u64 n, ...) {
	/* Variadic wrapper for usf_vstrcat */

	va_list args;
	va_start(args, n);

	i32 retval;
	retval = usf_vstrcat(destination, size, n, args);

	va_end(args);
	return retval;
}

i32 usf_vstrcat(char *destination, u64 size, u64 n, va_list args) {
	/* Concatenates n strings into destination (without the \0 terminator), which can hold
	 * size bytes at most. If the final string is longer than size (including the \0 terminator),
	 * this function returns -1. Otherwise, it returns 0. */

	va_list sizecheck;
	va_copy(sizecheck, args);

	u64 catsize, i;
	for (catsize = i = 0; i < n; i++) catsize += strlen(va_arg(sizecheck, char *));
	va_end(sizecheck);

	if (++catsize > size) return -1;

	strcpy(destination, va_arg(args, char *));
	for (i = 1; i < n; i++) strcat(destination, va_arg(args, char *));
	va_end(args);

	return 0;
}
