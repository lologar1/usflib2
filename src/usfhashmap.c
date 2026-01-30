#include "usfhashmap.h"

usf_hashmap *usf_newhm(void) {
	/* Wrapper for creating default-sized non thread-safe hashmaps. */

	return usf_newhmsz(USF_HASHMAP_DEFAULTSIZE);
}

usf_hashmap *usf_newhm_ts(void) {
	/* Wrapper for creating default-sized thread-safe hashmaps. */

	return usf_newhmsz_ts(USF_HASHMAP_DEFAULTSIZE);
}

usf_hashmap *usf_newhmsz(u64 capacity) {
	/* Creates a new usf_hashmap initialized to 0 of given capacity.
	 * Returns the created hashmap. */

	usf_hashmap *hashmap = usf_malloc(sizeof(usf_hashmap));
	hashmap->lock = NULL; /* Not thread-safe */
    hashmap->array = usf_calloc(capacity, sizeof(usf_data *));
	hashmap->size = 0; /* Empty at start */
	hashmap->capacity = capacity;

	return hashmap;
}

usf_hashmap *usf_newhmsz_ts(u64 capacity) {
	/* Creates a new thread-safe usf_hashmap initialized to 0 of given capacity.
	 * Returns the created hashmap, or NULL if a mutex cannot be created. */

	usf_hashmap *hashmap = usf_malloc(sizeof(usf_hashmap));
	hashmap->lock = usf_malloc(sizeof(pthread_mutex_t));
	if (pthread_mutex_init(hashmap->lock, NULL)) { /* Default attributes */
		usf_free(hashmap->lock);
		usf_free(hashmap);
		return NULL; /* mutex init failed */
	}
    hashmap->array = usf_calloc(capacity, sizeof(usf_data *));
	hashmap->size = 0; /* Empty at start */
	hashmap->capacity = capacity;

	return hashmap;
}

/* Common loop to find and access a hashmap element
 * _HASHMAP		reference to usf_hashmap *
 * _KEY			reference to the key (uint64_t or string)
 * _HASHFUNC	hashing function
 * _ACCESS		statements to execute for each potential entry
 *
 * _I			full 64-bit hash
 * _HASH		hashed index into underlying array
 * _CAP			hashmap capacity
 * _ENTRY		current hashmap entry being accessed
 * */

#define _USF_HMACCESS(_HASHMAP, _KEY, _HASHFUNC, _ACCESS) \
	u64 _I, _HASH, _CAP; \
	usf_data *_ENTRY; \
	_CAP = _HASHMAP->capacity; \
	_I = _HASHFUNC(_KEY); \
	\
	for (;; _I = usf_hash(_I)) { \
		_HASH = _I % _CAP; \
		_ENTRY = _HASHMAP->array[_HASH]; /* Get element */ \
		_ACCESS(_HASHMAP, _KEY, _HASHFUNC); \
	}

usf_hashmap *usf_strhmput(usf_hashmap *hashmap, const char *key, usf_data value) {
	/* Assigns a 64-bit usf_data value to this char *key.
	 * The key is hashed using usf_strhash.
	 * Returns the hashmap, or NULL on error. */

	if (hashmap == NULL || key == NULL) return NULL; /* Bad arguments */
	if (hashmap->lock) pthread_mutex_lock(hashmap->lock); /* Thread-safe lock */

	if (hashmap->size + 1 > hashmap->capacity / USF_HASHMAP_RESIZE_MULTIPLIER)
		usf_resizestrhm(hashmap, hashmap->capacity * USF_HASHMAP_RESIZE_MULTIPLIER);

#define _ACCESS(_HASHMAP, _KEY, _HASHFUNC) /* Uninitialized, deleted or matching entry */ \
	if (_ENTRY == NULL || _ENTRY[0].p == NULL || !strcmp((char *) _ENTRY[0].p, _KEY)) { \
		if (_ENTRY == NULL) /* Initialize entry */ \
			_ENTRY = _HASHMAP->array[_HASH] = usf_calloc(2, sizeof(usf_data)); \
		\
		if (_ENTRY[0].p == NULL) { /* Initialize key */ \
			_ENTRY[0] = USFDATAP(usf_malloc(strlen(_KEY) + 1)); \
			strcpy(_ENTRY[0].p, _KEY); \
			_HASHMAP->size++; \
		} \
		\
		_ENTRY[1] = value; \
		break; /* Successfully put */ \
	}
	_USF_HMACCESS(hashmap, key, usf_strhash, _ACCESS);
#undef _ACCESS

	if (hashmap->lock) pthread_mutex_unlock(hashmap->lock); /* Thread-safe unlock */
	return hashmap;
}

usf_hashmap *usf_inthmput(usf_hashmap *hashmap, u64 key, usf_data value) {
	/* Assigns a 64-bit usf_data value to this u64 key.
	 * The key is hashed using usf_strhash.
	 * Returns the hashmap, or NULL on error. */

	if (hashmap == NULL) return NULL;
	if (hashmap->lock) pthread_mutex_lock(hashmap->lock); /* Thread-safe lock */

	if (hashmap->size + 1 > hashmap->capacity / USF_HASHMAP_RESIZE_MULTIPLIER)
		usf_resizeinthm(hashmap, hashmap->capacity * USF_HASHMAP_RESIZE_MULTIPLIER);

#define _ACCESS(_HASHMAP, _KEY, _HASHFUNC) /* Uninitialized, deleted or matching entry */ \
	if (_ENTRY == NULL || (void *) _ENTRY == (void *) _HASHMAP || _ENTRY[0].u == _KEY) { \
		if (_ENTRY == NULL || _ENTRY == (usf_data *) _HASHMAP) { /* Empty or overwriting */ \
			_ENTRY = _HASHMAP->array[_HASH] = usf_calloc(2, sizeof(usf_data)); \
			_ENTRY[0] = USFDATAU(_KEY); \
			_HASHMAP->size++; \
		} \
		\
		_ENTRY[1] = value; \
		break; /* Successfully put */ \
	}
	_USF_HMACCESS(hashmap, key, usf_hash, _ACCESS);
#undef _ACCESS

	if (hashmap->lock) pthread_mutex_unlock(hashmap->lock); /* Thread-safe unlock */
	return hashmap;
}

usf_data usf_strhmget(usf_hashmap *hashmap, const char *key) {
	/* Returns the 64-bit usf_data value assigned to this char *key,
	 * or USFNULL (zero) if it is not accessible. */

	if (hashmap == NULL) return USFNULL;
	if (hashmap->lock) pthread_mutex_lock(hashmap->lock); /* Thread-safe lock */

#define _ACCESS(_HASHMAP, _KEY, _HASHFUNC) \
	if (_ENTRY == NULL) { \
		if (hashmap->lock) pthread_mutex_unlock(hashmap->lock); /* Thread-safe unlock */ \
		return USFNULL; /* Not present */ \
	} \
	\
	if (_ENTRY[0].p == NULL || strcmp((char *) _ENTRY[0].p, _KEY)) continue; /* Collision */ \
	\
	if (hashmap->lock) pthread_mutex_unlock(hashmap->lock); /* Thread-safe unlock */ \
	return _ENTRY[1]; /* Found */
	_USF_HMACCESS(hashmap, key, usf_strhash, _ACCESS);
#undef _ACCESS
}

usf_data usf_inthmget(usf_hashmap *hashmap, u64 key) {
	/* Returns the 64-bit usf_data value assigned to this u64 key,
	 * or USFNULL (zero) if it is not accessible. */

	if (hashmap == NULL) return USFNULL;
	if (hashmap->lock) pthread_mutex_lock(hashmap->lock); /* Thread-safe lock */

#define _ACCESS(_HASHMAP, _KEY, _HASHFUNC) \
	if (_ENTRY == NULL) { \
		if (hashmap->lock) pthread_mutex_unlock(hashmap->lock); /* Thread-safe unlock */ \
		return USFNULL; /* Not present */ \
	} \
	\
	if ((void *) _ENTRY == (void *) _HASHMAP || _ENTRY[0].u != _KEY) continue; /* Collision */ \
	\
	if (hashmap->lock) pthread_mutex_unlock(hashmap->lock); /* Thread-safe unlock */ \
	return _ENTRY[1]; /* Found */
	_USF_HMACCESS(hashmap, key, usf_hash, _ACCESS);
#undef _ACCESS
}

usf_data usf_strhmdel(usf_hashmap *hashmap, const char *key) {
	/* Delete the 64-bit usf_data value assigned to this char *key.
	 * Returns the deleted value, or if it is not accessible */

	if (hashmap == NULL) return USFNULL;
	if (hashmap->lock) pthread_mutex_lock(hashmap->lock); /* Thread-safe lock */

	usf_data value;
#define _ACCESS(_HASHMAP, _KEY, _HASHFUNC) \
	if (_ENTRY == NULL) { \
		if (hashmap->lock) pthread_mutex_unlock(hashmap->lock); /* Thread-safe unlock */ \
		return USFNULL; /* Not present */ \
	} \
	\
	if (_ENTRY[0].p == NULL || strcmp((char *) _ENTRY[0].p, _KEY)) continue; /* Collision */ \
	\
	_HASHMAP->size--; \
	value = _ENTRY[1]; \
	usf_free(_ENTRY[0].p); /* Free key */ \
	_ENTRY[0] = USFNULL; /* Delete entry */ \
	if (hashmap->lock) pthread_mutex_unlock(hashmap->lock); /* Thread-safe unlock */ \
	return value;
	_USF_HMACCESS(hashmap, key, usf_strhash, _ACCESS);
#undef _ACCESS
}

usf_data usf_inthmdel(usf_hashmap *hashmap, u64 key) {
	/* Delete the 64-bit usf_data value assigned to this u64 *key.
	 * Returns the deleted value, or if it is not accessible */

	if (hashmap == NULL) return USFNULL;
	if (hashmap->lock) pthread_mutex_lock(hashmap->lock); /* Thread-safe lock */

	usf_data value;
#define _ACCESS(_HASHMAP, _KEY, _HASHFUNC) \
	if (_ENTRY == NULL) { \
		if (hashmap->lock) pthread_mutex_unlock(hashmap->lock); /* Thread-safe unlock */ \
		return USFNULL; /* Not present */ \
	} \
	\
	if ((void *) _ENTRY == (void *) _HASHMAP || _ENTRY[0].u != _KEY) continue; /* Collision */ \
	\
	_HASHMAP->size--; \
	value = _ENTRY[1]; \
	usf_free(_ENTRY); /* Free entry */ \
	_HASHMAP->array[_HASH] = (usf_data *) _HASHMAP; /* Use hashmap pointer as deleted marker */ \
	if (hashmap->lock) pthread_mutex_unlock(hashmap->lock); /* Thread-safe unlock */ \
	return value;
	_USF_HMACCESS(hashmap, key, usf_hash, _ACCESS);
#undef _ACCESS
}

#undef _USF_HMACCESS

usf_data *usf_strhmnext(usf_hashmap *hashmap, u64 *iter) {
	/* Returns the next 64-bit usf_data value in the string hashmap from underlying array index iter.
	 * Then, increment iter. This functions is meant to iterate over a hashmap until iter >= capacity.
	 * If iter is invalid, USFNULL (zero) is returned.
	 * As this function is meant to be called multiple times, it is not inherently thread-safe.
	 * Users should instead manually lock the mutex when working with a thread-safe hashmap. */

	usf_data *entry;
	do {
		if (*iter >= hashmap->capacity) return NULL;
		entry = hashmap->array[(*iter)++];
	} while (entry == NULL || entry[0].p == NULL);

	return entry;
}

usf_data *usf_inthmnext(usf_hashmap *hashmap, u64 *iter) {
	/* Returns the next 64-bit usf_data value in the integer hashmap from underlying array index iter.
	 * Then, increment iter. This functions is meant to iterate over a hashmap until iter >= capacity.
	 * If iter is invalid, USFNULL (zero) is returned.
	 * As this function is meant to be called multiple times, it is not inherently thread-safe.
	 * Users should instead manually lock the mutex when working with a thread-safe hashmap. */

	usf_data *entry;
	do {
		if (*iter >= hashmap->capacity) return NULL;
		entry = hashmap->array[(*iter)++];
	} while (entry == NULL || (void *) entry == (void *) hashmap);

	return entry;
}

void usf_freestrhm(usf_hashmap *hashmap) {
	/* Frees a string usf_hashmap without calling usf_free on its values. */

	u64 i;
	usf_data **array, *entry;

	for (array = hashmap->array, i = 0; i < hashmap->capacity; i++) {
		if ((entry = array[i]) == NULL) continue; /* Uninitialized */
		if (entry[0].p) usf_free(entry[0].p); /* Free key */
		usf_free(entry); /* Free entry */
	}

	if (hashmap->lock) {
		pthread_mutex_destroy(hashmap->lock); /* Destroy pthread mutex in thread-safe hashmaps */
		usf_free(hashmap->lock);
	}
	usf_free(array); /* Free underlying array */
	usf_free(hashmap); /* Free hashmap struct */
}

void usf_freestrhmptr(usf_hashmap *hashmap) {
	/* Frees a string usf_hashmap and calls usf_free on its values. */

	u64 i;
	usf_data **array, *entry;

	for (array = hashmap->array, i = 0; i < hashmap->capacity; i++) {
		if ((entry = array[i]) == NULL) continue; /* Uninitialized */
		if (entry[0].p) {
			usf_free(entry[0].p); /* Free key */
			usf_free(entry[1].p); /* Free value */
		}
		usf_free(entry); /* Free entry */
	}

	if (hashmap->lock) {
		pthread_mutex_destroy(hashmap->lock); /* Destroy pthread mutex in thread-safe hashmaps */
		usf_free(hashmap->lock);
	}
	usf_free(array); /* Free underlying array */
	usf_free(hashmap); /* Free hashmap struct */
}

void usf_freeinthm(usf_hashmap *hashmap) {
	/* Frees an integer usf_hashmap without calling usf_free on its values.
	 * If hashmap is NULL, this function has no effect. */

	if (hashmap == NULL) return;

	u64 i;
	usf_data **array, *entry;
	for (array = hashmap->array, i = 0; i < hashmap->capacity; i++) {
		if ((entry = array[i]) == NULL) continue; /* Uninitialized */
		if ((void *) entry == (void *) hashmap) continue; /* Deleted */
		usf_free(entry); /* Free entry */
	}

	if (hashmap->lock) {
		pthread_mutex_destroy(hashmap->lock); /* Destroy pthread mutex in thread-safe hashmaps */
		usf_free(hashmap->lock);
	}
	usf_free(array); /* Free underlying array */
	usf_free(hashmap); /* Free hashmap struct */
}

void usf_freeinthmptr(usf_hashmap *hashmap) {
	/* Frees a string usf_hashmap and calls usf_free on its values.
	 * If hashmap is NULL, this function has no effect. */

	if (hashmap == NULL) return;

	u64 i;
	usf_data **array, *entry;
	for (array = hashmap->array, i = 0; i < hashmap->capacity; i++) {
		if ((entry = array[i]) == NULL) continue; /* Uninitialized */
		if ((void *) entry == (void *) hashmap) continue; /* Deleted */
		usf_free(entry[1].p); /* Free value */
		usf_free(entry); /* Free entry */
	}

	if (hashmap->lock) {
		pthread_mutex_destroy(hashmap->lock); /* Destroy pthread mutex in thread-safe hashmaps */
		usf_free(hashmap->lock);
	}
	usf_free(array); /* Free underlying array */
	usf_free(hashmap); /* Free hashmap struct */
}


void usf_resizeinthm(usf_hashmap *hashmap, u64 size) {
	/* Resizes the underlying array of an integer usf_hashmap to requested size in usf_data (8 bytes).
	 * If size is smaller or equal than the current size, nothing happens.
	 * as this function is internal to usf_hashmap's implementation, it is not inherently thread-safe.
	 * users should instead manually lock the mutex when working with a thread-safe hashmap. */

	if (hashmap == NULL || hashmap->capacity >= size) return; /* Bad arguments */

	usf_hashmap *newhm;
	newhm = usf_newhmsz(size);

	/* Move entries */
	u64 i = 0;
	usf_data *entry;
	while ((entry = usf_inthmnext(hashmap, &i)))
		usf_inthmput(newhm, entry[0].u, entry[1]);

	/* Swap arrays and capacity */
	USF_SWAP(hashmap->array, newhm->array);
	USF_SWAP(hashmap->capacity, newhm->capacity);
	usf_freeinthm(newhm); /* Free old data */
}

void usf_resizestrhm(usf_hashmap *hashmap, u64 size) {
	/* Resizes the underlying array of an integer usf_hashmap to requested size in usf_data (8 bytes).
	 * If size is smaller or equal than the current size, nothing happens.
	 * as this function is internal to usf_hashmap's implementation, it is not inherently thread-safe.
	 * users should instead manually lock the mutex when working with a thread-safe hashmap. */

	if (hashmap == NULL || hashmap->capacity >= size) return; /* Bad arguments */

	usf_hashmap *newhm;
	newhm = usf_newhmsz(size);

	/* Move entries */
	u64 i = 0;
	usf_data *entry;
	while ((entry = usf_strhmnext(hashmap, &i)))
		usf_strhmput(newhm, (char *) entry[0].p, entry[1]);

	/* Swap arrays and capacity */
	USF_SWAP(hashmap->array, newhm->array);
	USF_SWAP(hashmap->capacity, newhm->capacity);
	usf_freestrhm(newhm); /* Free old data */
}
