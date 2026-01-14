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
	void *p;
	u64 u;
	i64 i;
	f64 d;
} usf_data;

#endif
