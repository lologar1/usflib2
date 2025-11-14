#ifndef USFIO_H
#define USFIO_H

#include <stdio.h>
#include "usfstring.h"

char *usf_ftos(char *file, uint64_t *l);
void usf_printtxt(char **text, uint64_t len, FILE *stream);
char **usf_ftot(char *file, uint64_t *l);
void usf_freetxt(char **text, uint64_t nlines);
size_t usf_btof(char *file, void *pointer, size_t size);
void *usf_ftob(char *file, size_t *size);

#endif
