#ifndef USFIO_H
#define USFIO_H

#include <stdio.h>
#include "usfstd.h"
#include "usfstring.h"

char *usf_ftos(char *file, u64 *l);
char **usf_ftot(char *file, u64 *l);
char **usf_ftost(char *file, u64 *l);
void *usf_ftob(char *file, u64 *size);
u64 usf_btof(char *file, void *pointer, u64 size);
void usf_printtxt(char **text, u64 len);
void usf_fprinttxt(FILE *stream, char **text, u64 len);
void usf_freetxt(char **text, u64 nlines);

#endif
