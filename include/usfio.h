#ifndef USFIO_H
#define USFIO_H

#include <stdio.h>
#include "usfstd.h"
#include "usfstring.h"

char *usf_ftos(const char *file, u64 *l);
char **usf_ftot(const char *file, u64 *l);
char **usf_ftost(const char *file, u64 *l);
void *usf_ftob(const char *file, u64 *size);
u64 usf_btof(const char *file, const void *pointer, u64 size);
void usf_fprinttxt(FILE *stream, char *const *text, u64 len);	/* __REVISE__ cstyle@1.8.1 */
void usf_printtxt(char *const *text, u64 len);					/* __REVISE__ cstyle@1.8.1 */
void usf_freetxt(char **text, u64 nlines);

#endif
