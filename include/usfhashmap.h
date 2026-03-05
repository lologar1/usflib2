#ifndef USFHASHMAP_H
#define USFHASHMAP_H

#include <string.h>
#include "usfdata.h"
#include "usfmath.h"
#include "usfthread.h"

#define USF_HASHMAP_DEFAULTSIZE 16
#define USF_HASHMAP_RESIZE_MULTIPLIER 2

typedef enum usf_hashflag : u8 {
	USF_HASHMAP_UNINITIALIZED,
	USF_HASHMAP_SENTINEL,
	USF_HASHMAP_KEY_INTEGER,
	USF_HASHMAP_KEY_STRING
} usf_hashflag;

typedef struct usf_hashentry {
	usf_data key;
	usf_data value;
	usf_hashflag flag;
} usf_hashentry;

typedef struct usf_hashmap {
	usf_mutex *lock;
	usf_hashentry *array;
	u64 size;
	u64 capacity;
} usf_hashmap;

typedef struct usf_hashiter {
	u64 index;
	usf_hashentry *entry;
	usf_hashmap *hashmap;
} usf_hashiter;

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

void usf_hmiterstart(usf_hashmap *hashmap, usf_hashiter *iter);						/* Thread-safe */
usf_hashentry *usf_hmiternext(usf_hashiter *iter);
void usf_hmiterend(usf_hashiter *iter);												/* Thread-safe */

void usf_hmclear(usf_hashmap *hashmap);												/* Thread-safe */
void usf_resizehm(usf_hashmap *hashmap, u64 size);									/* Thread-safe */
void usf_freehmfunc(usf_hashmap *hashmap, void (*freefunc)(void *));
void usf_freehm(usf_hashmap *hashmap);

#endif
