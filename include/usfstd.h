#ifndef USFSTD_H
#define USFSTD_H

#include <stdlib.h>
#include <inttypes.h>
#include <stdatomic.h>
#if (__STDC_VERSION__ < 202311L) /* C23 adds native keyword support */
	#include <assert.h> /* static_assert */
	#include <threads.h> /* thread_local */
#endif

#if (__STDC_VERSION__ <= 202311L) /* Standards newer than C23 implement countof */
	#define countof(_ARRAY) (sizeof(_ARRAY)/sizeof(*_ARRAY))
#endif

#define USF_EMPTY

#define I8_MAX INT8_MAX
#define I8_MIN INT8_MIN
#define I16_MAX INT16_MAX
#define I16_MIN INT16_MIN
#define I32_MAX INT32_MAX
#define I32_MIN INT32_MIN
#define I64_MAX INT64_MAX
#define I64_MIN INT64_MIN
#define U8_MAX UINT8_MAX
#define U8_MIN 0
#define U16_MAX UINT16_MAX
#define U16_MIN 0
#define U32_MAX UINT32_MAX
#define U32_MIN 0
#define U64_MAX UINT64_MAX
#define U64_MIN 0

typedef int usf_compatibility_int;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

static_assert(sizeof(float) == 4 * sizeof(char), "usflib2: float type is not 4 bytes");
static_assert(sizeof(double) == 8 * sizeof(char), "usflib2: double type is not 8 bytes");
typedef float f32;
typedef double f64;

static_assert(((char) -1) < 0, "usflib2: char type is not signed");
static_assert(((u8) -1) == 255, "usflib2: negative representation is not two's complement");
static_assert(sizeof(void *) <= sizeof(u64), "usflib2: pointer type is bigger than u64");

#ifndef __STDC_NO_ATOMICS__
typedef _Atomic(i8) atomic_i8;
typedef _Atomic(i16) atomic_i16;
typedef _Atomic(i32) atomic_i32;
typedef _Atomic(i64) atomic_i64;
typedef _Atomic(u8) atomic_u8;

typedef _Atomic(u16) atomic_u16;
typedef _Atomic(u16) atomic_u32;
typedef _Atomic(u16) atomic_u64;
typedef _Atomic(f32) atomic_f32;
typedef _Atomic(f64) atomic_f64;
#else
	#warning "usflib2: warning: compiler does not provide atomic types"
#endif

/* libc wrappers for exact-width parsing */
#define strtoi32(_NPTR, _ENDPTR, _BASE) ((i32) strtol(_NPTR, _ENDPTR, _BASE))
#define strtoi64(_NPTR, _ENDPTR, _BASE) ((i64) strtoll(_NPTR, _ENDPTR, _BASE))
#define strtou32(_NPTR, _ENDPTR, _BASE) ((u32) strtoul(_NPTR, _ENDPTR, _BASE))
#define strtou64(_NPTR, _ENDPTR, _BASE) ((u64) strtoull(_NPTR, _ENDPTR, _BASE))

/* usflib2 allocation functions */
#define usf_malloc malloc
#define usf_calloc calloc
#define usf_alloca alloca
#define usf_realloc realloc
#define usf_free free

#endif
