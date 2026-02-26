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
#define _USF_INDCMPFUNC(_TYPE) \
	i32 usf_indcmp##_TYPE(const void *a, const void *b) { \
		_TYPE x = *((const _TYPE *) a); \
		_TYPE y = *((const _TYPE *) b); \
		return x > y ? 1 : x < y ? -1 : 0; \
	}
_USF_INDCMPFUNC(i32)
_USF_INDCMPFUNC(i64)
_USF_INDCMPFUNC(u32)
_USF_INDCMPFUNC(u64)
#undef _USF_INDCMPFUNC

#define _USF_ABSFUNC(_TYPE) \
	_TYPE usf_abs##_TYPE(_TYPE a) { return a < 0 ? -a : a; }
_USF_ABSFUNC(i32)
_USF_ABSFUNC(i64)
#undef _USF_ABSFUNC

#define _USF_MAXFUNC(_TYPE) \
	_TYPE usf_max##_TYPE(_TYPE a, _TYPE b) { return a > b ? a : b; }
_USF_MAXFUNC(i32)
_USF_MAXFUNC(i64)
_USF_MAXFUNC(u32)
_USF_MAXFUNC(u64)
#undef _USF_MAXFUNC

#define _USF_MINFUNC(_TYPE) \
	_TYPE usf_min##_TYPE(_TYPE a, _TYPE b) { return a < b ? a : b; }
_USF_MINFUNC(i32)
_USF_MINFUNC(i64)
_USF_MINFUNC(u32)
_USF_MINFUNC(u64)
#undef _USF_MINFUNC

#define _USF_CLAMPFUNC(_TYPE) \
	_TYPE usf_clamp##_TYPE(_TYPE x, _TYPE low, _TYPE high) { \
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
