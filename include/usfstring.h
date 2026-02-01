#ifndef USFSTRING_H
#define USFSTRING_H

#include <string.h>
#include <stdarg.h>
#include "usfstd.h"
#include "usfmath.h"

i32 usf_indstrcmp(const void *a, const void *b);
i32 usf_indstrlen(const void *a, const void *b);

const char *usf_sstartswith(const char *base, const char *prefix);
const char *usf_sendswith(const char *base, const char *suffix);
u64 usf_scount(const char *str, char c);
u64 usf_sreplace(char *str, char from, char to);
char **usf_scsplit(char *str, char sep, u64 *count);
void usf_supper(char *str);
void usf_slower(char *str);

i32 usf_txtcontainsline(char *const *array, u64 len, const char *string); /* __REVISE__ cstyle@1.8.1 */
void usf_reversetxtlines(char **array, u64 len);
i32 usf_strcat(char *destination, u64 size, u64 n, ...);
i32 usf_vstrcat(char *destination, u64 size, u64 n, va_list args);

#endif
