#include "usfmath.h"

/* General purpose file for miscellaneous functions */

double usf_elapsedtimes(struct timespec start, struct timespec end) {
	/* Adjust time to nanosecond precision */
	return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000LU;
}

double usf_elapsedtimens(struct timespec start, struct timespec end) {
	return (end.tv_sec - start.tv_sec) * 1000000000LU + (end.tv_nsec - start.tv_nsec);
}
