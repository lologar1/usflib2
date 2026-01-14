#ifndef USFSTRING_H
#define USFSTRING_H

#include <string.h>
#include <stdarg.h>
#include "usfstd.h"
#include "usfmath.h"

int usf_indstrcmp(const void *a, const void *b);
int usf_indstrlen(const void *a, const void *b);

char *usf_sstartswith(char *base, char *prefix);
char *usf_sendswith(char *base, char *suffix);
u64 usf_scount(char *str, char c);
i32 usf_txtcontainsline(char **array, u64 len, char *string);
void usf_reversetxtlines(char **array, u64 len);
u64 usf_sreplace(char *str, char tmpl, char replacement);
char **usf_scsplit(char *str, char sep, u64 *count);
i32 usf_strcat(char *destination, u64 size, u64 n, ...);
i32 usf_vstrcat(char *destination, u64 size, u64 n, va_list args);

#endif
