#ifndef USFHASHMAP_H
#define USFHASHMAP_H

#include <string.h>
#include <pthread.h>
#include "usfdata.h"
#include "usfmath.h"

#define USF_HASHMAP_DEFAULTSIZE 16
#define USF_HASHMAP_RESIZE_MULTIPLIER 2

typedef struct usf_hashmap {
	pthread_mutex_t *lock;
	usf_data **array; /* Hash : Value */
	u64 size;
	u64 capacity;
} usf_hashmap;

/* General hashmap functions */
usf_hashmap *usf_newhm(void);
usf_hashmap *usf_newhm_ts(void);
usf_hashmap *usf_newhmsz(u64 capacity);
usf_hashmap *usf_newhmsz_ts(u64 capacity);
usf_hashmap *usf_strhmput(usf_hashmap *hashmap, char *key, usf_data value);
usf_hashmap *usf_inthmput(usf_hashmap *hashmap, u64 key, usf_data value);
usf_data usf_strhmget(usf_hashmap *hashmap, char *key);
usf_data usf_inthmget(usf_hashmap *hashmap, u64 key);
usf_data usf_strhmdel(usf_hashmap *hashmap, char *key);
usf_data usf_inthmdel(usf_hashmap *hashmap, u64 key);
usf_data *usf_strhmnext(usf_hashmap *hashmap, u64 *iter);
usf_data *usf_inthmnext(usf_hashmap *hashmap, u64 *iter);
void usf_freestrhm(usf_hashmap *hashmap);
void usf_freestrhmptr(usf_hashmap *hashmap);
void usf_freeinthm(usf_hashmap *hashmap);
void usf_freeinthmptr(usf_hashmap *hashmap);

/* Internal */
void usf_resizestrhm(usf_hashmap *hashmap, u64 size);
void usf_resizeinthm(usf_hashmap *hashmap, u64 size);

#endif
