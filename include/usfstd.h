#ifndef USFSTD_H
#define USFSTD_H

#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>

#define USF_EMPTY

/* Limit macros */
#define I8_MAX ((i8) 127LL)
#define I8_MIN ((i8) -128LL)
#define I16_MAX ((i16) 32767LL)
#define I16_MIN ((i16) -32768LL)
#define I32_MAX ((i32) 2147483647LL)
#define I32_MIN ((i32) -2147483648LL)
#define I64_MAX ((i64) 9223372036854775807LL)
#define I64_MIN ((i64) -9223372036854775808LL)
#define U8_MAX ((u8) 255ULL)
#define U8_MIN (0)
#define U16_MAX ((u16) 65535ULL)
#define U16_MIN (0)
#define U32_MAX ((u32) 4294967295ULL)
#define U32_MIN (0)
#define U64_MAX ((u64) 18446744073709551615ULL)
#define U64_MIN (0)

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
