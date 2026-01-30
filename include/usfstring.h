#ifndef USFSTRING_H
#define USFSTRING_H

#include <string.h>
#include <stdarg.h>
#include "usfstd.h"
#include "usfmath.h"

int usf_indstrcmp(const void *a, const void *b);
int usf_indstrlen(const void *a, const void *b);

const char *usf_sstartswith(const char *base, const char *prefix);
const char *usf_sendswith(const char *base, const char *suffix);
u64 usf_scount(const char *str, char c);
i32 usf_txtcontainsline(const char **array, u64 len, const char *string);
void usf_reversetxtlines(char **array, u64 len);
u64 usf_sreplace(char *str, char tmpl, char replacement);
char **usf_scsplit(char *str, char sep, u64 *count);
i32 usf_strcat(char *destination, u64 size, u64 n, ...);
i32 usf_vstrcat(char *destination, u64 size, u64 n, va_list args);
void usf_supper(char *str);
void usf_slower(char *str);

#endif
