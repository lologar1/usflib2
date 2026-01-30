#ifndef USFDATA_H
#define USFDATA_H

#include "usfstd.h"

#define USFNULL ((usf_data) { .u = 0 })
#define USFTRUE ((usf_data) { .u = 1 })
#define USFDATAP(_D) ((usf_data) { .ptr = (_D) })
#define USFDATAU(_D) ((usf_data) { .u64 = (_D) })
#define USFDATAI(_D) ((usf_data) { .i64 = (_D) })
#define USFDATAD(_D) ((usf_data) { .f64 = (_D) })

typedef union usf_data {
	u64 u;
	u64 u64;
	i64 i;
	i64 i64;
	f64 f;
	f64 f64;
	void *p;
	void *ptr;
} usf_data;

#endif
