#ifndef USFMATH_H
#define USFMATH_H

#include <math.h>
#include <time.h>
#include "usfstd.h"

u64 usf_strhash(const char *str);
u64 usf_hash(u64 val);

f64 usf_elapsedtimes(struct timespec start, struct timespec end);
f64 usf_elapsedtimens(struct timespec start, struct timespec end);

#define USF_SWAP(A, B) do { \
	__typeof__(A) _TMP = (A); \
	(A) = (B); (B) = _TMP; \
} while (0)

#define USF_MAX(A, B) \
	_Generic((A), \
			f32: fmaxf, \
			f64: fmax, \
			i32: usf_maxi32, \
			u32: usf_maxu32, \
			i64: usf_maxi64, \
			u64: usf_maxu64 \
		)((A), (B))

#define USF_MIN(A, B) \
	_Generic((A), \
			f32: fminf, \
			f64: fmin, \
			i32: usf_mini32, \
			u32: usf_minu32, \
			i64: usf_mini64, \
			u64: usf_minu64 \
		)((A), (B))

#define USF_CLAMP(X, LOW, HIGH) \
	_Generic((X), \
			f32: usf_clampf32, \
			f64: usf_clampf64, \
			i32: usf_clampi32, \
			u32: usf_clampu32, \
			i64: usf_clampi64, \
			u64: usf_clampu64 \
		)((X), (LOW), (HIGH))

/* Generic comparison functions */
i32 usf_indcmpi32(const void *a, const void *b);
i32 usf_indcmpi64(const void *a, const void *b);
i32 usf_indcmpu32(const void *a, const void *b);
i32 usf_indcmpu64(const void *a, const void *b);
i32 usf_maxi32(i32 a, i32 b);
i64 usf_maxi64(i64 a, i64 b);
u32 usf_maxu32(u32 a, u32 b);
u64 usf_maxu64(u64 a, u64 b);
i32 usf_mini32(i32 a, i32 b);
i64 usf_mini64(i64 a, i64 b);
u32 usf_minu32(u32 a, u32 b);
u64 usf_minu64(u64 a, u64 b);
f32 usf_clampf32(f32 x, f32 low, f32 high);
f64 usf_clampf64(f64 x, f64 low, f64 high);
i32 usf_clampi32(i32 x, i32 low, i32 high);
i64 usf_clampi64(i64 x, i64 low, i64 high);
u32 usf_clampu32(u32 x, u32 low, u32 high);
u64 usf_clampu64(u64 x, u64 low, u64 high);

#endif
