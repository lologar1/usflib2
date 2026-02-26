#ifndef USFTIME_H
#define USFTIME_H

#include <time.h>
#include "usfstd.h"

typedef struct timespec timespec;

f64 usf_elapsedtimes(timespec start, timespec end);		/* DEPRECATED! */
f64 usf_elapsedtimens(timespec start, timespec end);	/* DEPRECATED! */

void usf_tsdiff(const timespec *a, const timespec *b, timespec *result);

#endif
