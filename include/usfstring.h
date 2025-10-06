#ifndef USFSTRING_H
#define USFSTRING_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

int usf_indstrcmp(const void *, const void *);
int usf_indstrlen(const void *, const void *);
char *usf_sstartswith(char *base, char *prefix);
int usf_sendswith(char *base, char *suffix);
uint64_t usf_scount(char *str, char c);
int usf_txtcontainsline(char **array, uint64_t len, char *string);
void usf_reversetxtlines(char **array, uint64_t len);
int usf_sreplace(char *s, char tmpl, char replacement);
char **usf_scsplit(char *str, char sep, uint64_t *count);

#endif
