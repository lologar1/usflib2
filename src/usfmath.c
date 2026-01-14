#include "usfmath.h"

u64 usf_strhash(const char *str) {
	/* Returns a 64-bit unsigned hash of the given string. */

    u64 hash = 5381, c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

u64 usf_hash(u64 val) {
	/* Returns a 64-bit unsigned hash of the given 64-bit unsigned value. */

	val += 137;
	val ^= val >> 33;
	val *= 0xFF51AFD7ED558CCD; //Prime
	val ^= val >> 31;
	val *= 0xA635194A4D16E3CB; //Prime
	val ^= val >> 27;
	return val;
}

f64 usf_elapsedtimes(struct timespec start, struct timespec end) {
	/* Return time elapsed between start and end with nanosecond precision, in seconds. */

	return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000LU;
}

f64 usf_elapsedtimens(struct timespec start, struct timespec end) {
	/* Return time elapsed between start and end with nanosecond precision, in nanoseconds. */

	return (end.tv_sec - start.tv_sec) * 1000000000LU + (end.tv_nsec - start.tv_nsec);
}

/* Generic comparison functions */
#define _USF_INDCMPFUNC(TYPE) \
	i32 usf_indcmp##TYPE(const void *a, const void *b) { \
		TYPE x = *((TYPE *) a); \
		TYPE y = *((TYPE *) b); \
		return x > y ? 1 : x < y ? -1 : 0; \
	}
_USF_INDCMPFUNC(i32)
_USF_INDCMPFUNC(i64)
_USF_INDCMPFUNC(u32)
_USF_INDCMPFUNC(u64)
#undef _USF_INDCMPFUNC

#define _USF_MAXFUNC(TYPE) \
	TYPE usf_max##TYPE(TYPE a, TYPE b) { return a > b ? a : b; }
_USF_MAXFUNC(i32)
_USF_MAXFUNC(i64)
_USF_MAXFUNC(u32)
_USF_MAXFUNC(u64)
#undef _USF_MAXFUNC

#define _USF_MINFUNC(TYPE) \
	TYPE usf_min##TYPE(TYPE a, TYPE b) { return a < b ? a : b; }
_USF_MINFUNC(i32)
_USF_MINFUNC(i64)
_USF_MINFUNC(u32)
_USF_MINFUNC(u64)
#undef _USF_MINFUNC

#define _USF_CLAMPFUNC(TYPE) \
	TYPE usf_clamp##TYPE(TYPE x, TYPE low, TYPE high) { \
		if (low > high) USF_SWAP(low, high); \
		return x < low ? low : (x > high ? high : x); \
	}
_USF_CLAMPFUNC(f32)
_USF_CLAMPFUNC(f64)
_USF_CLAMPFUNC(i32)
_USF_CLAMPFUNC(i64)
_USF_CLAMPFUNC(u32)
_USF_CLAMPFUNC(u64)
#undef _USF_CLAMPFUNC
