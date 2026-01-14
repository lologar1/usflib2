#ifndef USFDATA_H
#define USFDATA_H

#include "usfstd.h"

#define USFNULL ((usf_data) { .u = 0 })
#define USFTRUE ((usf_data) { .u = 1 })
#define USFDATAP(d) ((usf_data) { .p = (d) })
#define USFDATAU(d) ((usf_data) { .u = (d) })
#define USFDATAI(d) ((usf_data) { .i = (d) })
#define USFDATAD(d) ((usf_data) { .d = (d) })

typedef union usf_data {
	void *p;
	u64 u;
	i64 i;
	f64 d;
} usf_data;

#endif
