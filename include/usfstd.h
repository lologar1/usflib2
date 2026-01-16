#ifndef USFSTD_H
#define USFSTD_H

#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>

#define USF_EMPTY

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

#define usf_malloc malloc
#define usf_calloc calloc
#define usf_alloca alloca
#define usf_realloc realloc
#define usf_free free

#endif
