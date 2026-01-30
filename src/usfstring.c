#include "usfstring.h"

static const char USF_CHAR16LOWER[] = "é\0è\0à\0ù\0â\0ê\0î\0ô\0û\0ë\0ï\0ü\0ÿ\0ç\0œ\0æ\0å\0ä\0ö\0";
static const char USF_CHAR16UPPER[] = "É\0È\0À\0Ù\0Â\0Ê\0Î\0Ô\0Û\0Ë\0Ï\0Ü\0Ÿ\0Ç\0Œ\0Æ\0Å\0Ä\0Ö\0";

int usf_indstrcmp(const void *a, const void *b) {
	/* strcmp wrapper for qsort */

	const char *const *x = (const char *const *) a;
	const char *const *y = (const char *const *) b;

	if (*x == NULL) {
		if (*y == NULL)
			return 0;
		return -1;
	} else if (*y == NULL)
		return 1;

	return strcmp(*x, *y);
}

int usf_indstrlen(const void *a, const void *b) {
	/* strlen wrapper for qsort */

	const char *const *x = (const char *const *) a;
	const char *const *y = (const char *const *) b;

	if (*x == NULL) {
		if (*y == NULL)
			return 0;
		return -1;
	} else if (*y == NULL)
		return 1;

	return strlen(*x) - strlen(*y);
}

const char *usf_sstartswith(const char *base, const char *prefix) {
	/* Returns a pointer to the first char in base after the prefix (excluding terminating \0),
	 * or NULL if base does not start with prefix. */

	if (base == NULL || prefix == NULL) return NULL;

	while (*prefix) /* If a char in prefix mismatches, condition failed */
		if (*prefix++ != *base++) return NULL;

	return base;
}

const char *usf_sendswith(const char *base, const char *suffix) {
	/* Returns a pointer to the first char in base before the suffix,
	 * or NULL if base does not end with suffix. */

	i64 offset;
	if ((offset = ((i64) strlen(base) - (i64) strlen(suffix))) < 0) return NULL;

	const char *start;
	start = base + offset;

	return strcmp(start, suffix) ? NULL : start;
}

u64 usf_scount(const char *str, char c) {
	/* Returns the number of occurrences of char c in string str. */

	u64 count;
	for (count = 0; *str; str++)
		if (*str == c) count++;

	return count;
}

i32 usf_txtcontainsline(const char **array, u64 len, const char *string) {
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

void usf_supper(char *str) {
	/* Makes a string uppercase; also has support for all French and Swedish special characters
	 * éèàùâêîôûëïüÿçœæåäö -> ÉÈÀÙÂÊÎÔÛËÏÜŸÇŒÆÅÄÖ */

	char *c;
	const char *c16;
	for (c = str; *c; c++) {
		if (*c >= 'a' && *c <= 'z') *c -= ('a' - 'A');
		else for (c16 = USF_CHAR16LOWER; *c16; c16 += 3) if (usf_sstartswith(c, c16)) {
			memcpy(c, &USF_CHAR16UPPER[c16 - USF_CHAR16LOWER], 2); /* Overwrite with uppercase char16 */
			c++; /* Skip second part of 2-byte char */
			break;
		}
	}
}

void usf_slower(char *str) {
	/* Makes a string lowercase; also has support for all French and Swedish special characters
	 * ÉÈÀÙÂÊÎÔÛËÏÜŸÇŒÆÅÄÖ -> éèàùâêîôûëïüÿçœæåäö */

	char *c;
	const char *c16;
	for (c = str; *c; c++) {
		if (*c >= 'A' && *c <= 'Z') *c += ('a' - 'A');
		else for (c16 = USF_CHAR16UPPER; *c16; c16 += 3) if (usf_sstartswith(c, c16)) {
			memcpy(c, &USF_CHAR16LOWER[c16 - USF_CHAR16UPPER], 2); /* Overwrite with lowercase char16 */
			c++; /* Skip second part of 2-byte char */
			break;
		}
	}
}
