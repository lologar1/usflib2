#ifndef USFDATA_H
#define USFDATA_H

#include "usfstd.h"

#define USFNULL ((usf_data) { .u = 0 })
#define USFTRUE ((usf_data) { .u = 1 })
#define USFDATAP(D) ((usf_data) { .p = (D) })
#define USFDATAU(D) ((usf_data) { .u = (D) })
#define USFDATAI(D) ((usf_data) { .i = (D) })
#define USFDATAD(D) ((usf_data) { .d = (D) })

typedef union usf_data {
	u64 u;
	u64 u64;
	i64 i;
	i64 i64;
	f64 d;
	f64 f64;
	void *p;
} usf_data;

#endif
