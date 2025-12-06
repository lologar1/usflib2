#ifndef USFSTRING_H
#define USFSTRING_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

int usf_indstrcmp(const void *a, const void *b);
int usf_indstrlen(const void *a, const void *b);
char *usf_sstartswith(char *base, char *prefix);
int usf_sendswith(char *base, char *suffix);
uint64_t usf_scount(char *str, char c);
int usf_txtcontainsline(char **array, uint64_t len, char *string);
void usf_reversetxtlines(char **array, uint64_t len);
int usf_sreplace(char *s, char tmpl, char replacement);
char **usf_scsplit(char *str, char sep, uint64_t *count);
int usf_strcat(char *destination, size_t size, uint32_t n, ...);
int usf_vstrcat(char *destination, size_t size, uint32_t n, va_list args);

#endif
