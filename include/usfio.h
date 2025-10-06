#ifndef USFIO_H
#define USFIO_H

#include <stdio.h>
#include "usfstring.h"

char *usf_ftos(char *file, char *options, uint64_t *l);
void usf_printtxt(char **text, uint64_t len, FILE *stream);
char **usf_ftot(char *file, char *options, uint64_t *l);
void usf_freetxt(char **text, uint64_t nlines);

#endif
