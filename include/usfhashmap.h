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

usf_hashmap *usf_newhm(void);
usf_hashmap *usf_newhm_ts(void);
usf_hashmap *usf_newhmsz(u64 capacity);
usf_hashmap *usf_newhmsz_ts(u64 capacity);

usf_hashmap *usf_strhmput(usf_hashmap *hashmap, const char *key, usf_data value);	/* Thread-safe */
usf_data usf_strhmget(const usf_hashmap *hashmap, const char *key);					/* Thread-safe */
usf_data usf_strhmdel(usf_hashmap *hashmap, const char *key);						/* Thread-safe */
usf_hashmap *usf_inthmput(usf_hashmap *hashmap, u64 key, usf_data value);			/* Thread-safe */
usf_data usf_inthmget(const usf_hashmap *hashmap, u64 key);							/* Thread-safe */
usf_data usf_inthmdel(usf_hashmap *hashmap, u64 key);								/* Thread-safe */

usf_data *usf_strhmnext(const usf_hashmap *hashmap, u64 *iter);
void usf_freestrhmfunc(usf_hashmap *hashmap, void (*freefunc)(void *));
void usf_freestrhmptr(usf_hashmap *hashmap);
void usf_freestrhm(usf_hashmap *hashmap);
usf_data *usf_inthmnext(const usf_hashmap *hashmap, u64 *iter);
void usf_freeinthmfunc(usf_hashmap *hashmap, void (*freefunc)(void *));
void usf_freeinthmptr(usf_hashmap *hashmap);
void usf_freeinthm(usf_hashmap *hashmap);

void usf_resizestrhm(usf_hashmap *hashmap, u64 size);
void usf_resizeinthm(usf_hashmap *hashmap, u64 size);

#endif
