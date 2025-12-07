#include "usfmath.h"

/* General purpose file for miscellaneous functions */

double usf_elapsedtimes(struct timespec start, struct timespec end) {
	/* Adjust time to nanosecond precision */
	return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000LU;
}

double usf_elapsedtimens(struct timespec start, struct timespec end) {
	return (end.tv_sec - start.tv_sec) * 1000000000LU + (end.tv_nsec - start.tv_nsec);
}

int32_t usf_maxi32(int32_t a, int32_t b) { return a > b ? a : b; }
uint32_t usf_maxu32(uint32_t a, uint32_t b) { return a > b ? a : b; }
int64_t usf_maxi64(int64_t a, int64_t b) { return a > b ? a : b; }
uint64_t usf_maxu64(uint64_t a, uint64_t b) { return a > b ? a : b; }
int32_t usf_mini32(int32_t a, int32_t b) { return a < b ? a : b; }
uint32_t usf_minu32(uint32_t a, uint32_t b) { return a < b ? a : b; }
int64_t usf_mini64(int64_t a, int64_t b) { return a < b ? a : b; }
uint64_t usf_minu64(uint64_t a, uint64_t b) { return a < b ? a : b; }
float usf_clampf(float x, float low, float high) {
	if (low > high) USF_SWAP(low, high);
	return x < low ? low : (x > high ? high : x);
}
double usf_clampd(double x, double low, double high) {
	if (low > high) USF_SWAP(low, high);
	return x < low ? low : (x > high ? high : x);
}
int32_t usf_clampi32(int32_t x, int32_t low, int32_t high) {
	if (low > high) USF_SWAP(low, high);
	return x < low ? low : (x > high ? high : x);
}
uint32_t usf_clampu32(uint32_t x, uint32_t low, uint32_t high) {
	if (low > high) USF_SWAP(low, high);
	return x < low ? low : (x > high ? high : x);
}
int64_t usf_clampi64(int64_t x, int64_t low, int64_t high) {
	if (low > high) USF_SWAP(low, high);
	return x < low ? low : (x > high ? high : x);
}
uint64_t usf_clampu64(uint64_t x, uint64_t low, uint64_t high) {
	if (low > high) USF_SWAP(low, high);
	return x < low ? low : (x > high ? high : x);
}
