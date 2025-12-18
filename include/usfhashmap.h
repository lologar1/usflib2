#ifndef USFHASHMAP_H
#define USFHASHMAP_H

#include "usfdata.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define USF_HASHMAP_DEFAULTSIZE 16
#define USF_HASHMAP_RESIZE_MULTIPLIER 2

uint64_t usf_strhash(const char *str);
uint64_t usf_hash(uint64_t val);

typedef struct usf_hashmap {
	usf_data **array; //Hash : value
	uint64_t size;
	uint64_t capacity;
} usf_hashmap;

usf_hashmap *usf_newhm(void);
usf_hashmap *usf_strhmput(usf_hashmap *hashmap, char *key, usf_data value);
usf_hashmap *usf_inthmput(usf_hashmap *hashmap, uint64_t key, usf_data value);
usf_data usf_strhmget(usf_hashmap *hashmap, char *key);
usf_data usf_inthmget(usf_hashmap *hashmap, uint64_t key);
usf_data usf_strhmdel(usf_hashmap *hashmap, char *key);
usf_data usf_inthmdel(usf_hashmap *hashmap, uint64_t key);
usf_data *usf_strhmnext(usf_hashmap *hashmap, uint64_t *iter);
usf_data *usf_inthmnext(usf_hashmap *hashmap, uint64_t *iter);
void usf_resizestrhm(usf_hashmap *, uint64_t size);
void usf_resizeinthm(usf_hashmap *hashmap, uint64_t size);
void usf_freestrhm(usf_hashmap *hashmap);
void usf_freestrhmptr(usf_hashmap *hashmap);
void usf_freehm(usf_hashmap *hashmap);
void usf_freehmptr(usf_hashmap *hashmap);

#endif
