#include "usfmath.h"

u64 usf_strhash(const char *str) {
	/* Returns a 64-bit unsigned hash of the given string. */

    u64 hash = 5381, c;
    while ((c = (u64) *str++)) {
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

/* Generic comparison functions */
#define USF_INDCMPFUNC(_TYPE) \
	i32 usf_indcmp##_TYPE(const void *a, const void *b) { \
		_TYPE x = *((const _TYPE *) a); \
		_TYPE y = *((const _TYPE *) b); \
		return x > y ? 1 : x < y ? -1 : 0; \
	}
USF_INDCMPFUNC(i32)
USF_INDCMPFUNC(i64)
USF_INDCMPFUNC(u32)
USF_INDCMPFUNC(u64)
#undef USF_INDCMPFUNC

#define USF_ABSFUNC(_TYPE) \
	_TYPE usf_abs##_TYPE(_TYPE a) { return a < 0 ? -a : a; }
USF_ABSFUNC(i32)
USF_ABSFUNC(i64)
#undef USF_ABSFUNC

#define USF_MAXFUNC(_TYPE) \
	_TYPE usf_max##_TYPE(_TYPE a, _TYPE b) { return a > b ? a : b; }
USF_MAXFUNC(i32)
USF_MAXFUNC(i64)
USF_MAXFUNC(u32)
USF_MAXFUNC(u64)
#undef USF_MAXFUNC

#define USF_MINFUNC(_TYPE) \
	_TYPE usf_min##_TYPE(_TYPE a, _TYPE b) { return a < b ? a : b; }
USF_MINFUNC(i32)
USF_MINFUNC(i64)
USF_MINFUNC(u32)
USF_MINFUNC(u64)
#undef USF_MINFUNC

#define USF_CLAMPFUNC(_TYPE) \
	_TYPE usf_clamp##_TYPE(_TYPE x, _TYPE low, _TYPE high) { \
		if (low > high) USF_SWAP(low, high); \
		return x < low ? low : (x > high ? high : x); \
	}
USF_CLAMPFUNC(f32)
USF_CLAMPFUNC(f64)
USF_CLAMPFUNC(i32)
USF_CLAMPFUNC(i64)
USF_CLAMPFUNC(u32)
USF_CLAMPFUNC(u64)
#undef USF_CLAMPFUNC
