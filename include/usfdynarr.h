#ifndef USFDYNARR_H
#define USFDYNARR_H

#include "usfdata.h"
#include <stdlib.h>
#include <string.h>

typedef struct usf_dynarr {
	usf_data *array;
	uint64_t capacity; /* Real array size */
	uint64_t size; /* Virtual array size */
} usf_dynarr;

usf_dynarr *usf_newda(uint64_t size);
usf_dynarr *usf_arrtodyn(usf_data *arr, uint64_t size);
usf_data usf_daappend(usf_dynarr *da, usf_data data);
usf_data usf_daget(usf_dynarr *da, uint64_t index);
usf_data usf_daset(usf_dynarr *da, uint64_t index, usf_data data);
void usf_freeda(usf_dynarr *da);


#endif
