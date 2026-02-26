#include "usftime.h"

f64 usf_elapsedtimes(timespec start, timespec end) { /* THIS FUNCTION IS DEPRECATED */
	/* Return time elapsed between start and end with nanosecond precision, in seconds. */

	return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

f64 usf_elapsedtimens(timespec start, timespec end) { /* THIS FUNCTION IS DEPRECATED */
	/* Return time elapsed between start and end with nanosecond precision, in nanoseconds. */

	return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

void usf_tsdiff(const timespec *a, const timespec *b, timespec *result) {
	/* Subtract two timespecs, assuming that a comes before b */

	i64 secdiff, nanodiff;

	secdiff = a->tv_sec - b->tv_sec;
	nanodiff = a->tv_nsec - b->tv_nsec;

	if (nanodiff < 0) {
		secdiff--;
		nanodiff += 1000000000; /* 1e9 nanoseconds per second */
	}

	result->tv_sec = secdiff;
	result->tv_nsec = nanodiff;
}
