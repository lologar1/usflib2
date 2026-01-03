#ifndef USFMATH_H
#define USFMATH_H

#include <stdint.h>
#include <math.h>
#include <time.h>

double usf_elapsedtimes(struct timespec start, struct timespec end);
double usf_elapsedtimens(struct timespec start, struct timespec end);

#define USF_SWAP(A, B) do { \
	__typeof__(A) _TMP = (A); \
	(A) = (B); (B) = _TMP; \
} while (0)

#define USF_MAX(A, B) \
	_Generic((A), \
			float: fmaxf, \
			double: fmax, \
			int32_t: usf_maxi32, \
			uint32_t: usf_maxu32, \
			int64_t: usf_maxi64, \
			uint64_t: usf_maxu64 \
		)((A), (B))

#define USF_MIN(A, B) \
	_Generic((A), \
			float: fminf, \
			double: fmin, \
			int32_t: usf_mini32, \
			uint32_t: usf_minu32, \
			int64_t: usf_mini64, \
			uint64_t: usf_minu64 \
		)((A), (B))

#define USF_CLAMP(X, LOW, HIGH) \
	_Generic((X), \
			float: usf_clampf, \
			double: usf_clampd, \
			int32_t: usf_clampi32, \
			uint32_t: usf_clampu32, \
			int64_t: usf_clampi64, \
			uint64_t: usf_clampu64 \
		)((X), (LOW), (HIGH))

uint64_t usf_strhash(const char *str);
uint64_t usf_hash(uint64_t val);

int32_t usf_indi32cmp(const void *a, const void *b);
int32_t usf_indu32cmp(const void *a, const void *b);
int32_t usf_indi64cmp(const void *a, const void *b);
int32_t usf_indu64cmp(const void *a, const void *b);

int32_t usf_maxi32(int32_t a, int32_t b);
uint32_t usf_maxu32(uint32_t a, uint32_t b);
int64_t usf_maxi64(int64_t a, int64_t b);
uint64_t usf_maxu64(uint64_t a, uint64_t b);
int32_t usf_mini32(int32_t a, int32_t b);
uint32_t usf_minu32(uint32_t a, uint32_t b);
int64_t usf_mini64(int64_t a, int64_t b);
uint64_t usf_minu64(uint64_t a, uint64_t b);
float usf_clampf(float x, float low, float high);
double usf_clampd(double x, double low, double high);
int32_t usf_clampi32(int32_t x, int32_t low, int32_t high);
uint32_t usf_clampu32(uint32_t x, uint32_t low, uint32_t high);
int64_t usf_clampi64(int64_t x, int64_t low, int64_t high);
uint64_t usf_clampu64(uint64_t x, uint64_t low, uint64_t high);
#endif
