#ifndef USFMATH_H
#define USFMATH_H

#include <time.h>

#define USF_MIN(X, Y) ((X) <= (Y) ? (X) : (Y))
#define USF_MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define USF_CLAMP(X, LOW, HIGH) ((X) < (LOW) ? (LOW) : ((X) > (HIGH) ? (HIGH) : (X)))

double usf_elapsedtimes(struct timespec start, struct timespec end);
double usf_elapsedtimens(struct timespec start, struct timespec end);

#endif
