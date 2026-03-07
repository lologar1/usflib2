#include "usfhashmap.h"

usf_hashmap *usf_newhm(void) {
	/* Wrapper for creating default-sized non-blocking hashmaps. */

	return usf_newhmsz(USF_HASHMAP_DEFAULTSIZE);
}

usf_hashmap *usf_newhm_mtx(void) {
	/* Wrapper for creating default-sized thread-blocking hashmaps. */

	return usf_newhmsz_mtx(USF_HASHMAP_DEFAULTSIZE);
}

usf_hashmap *usf_newhmsz(u64 capacity) {
	/* Creates a new non-blocking usf_hashmap initialized to 0 of given capacity.
	 * Returns the created hashmap. */

	usf_hashmap *hashmap;
	hashmap = usf_malloc(sizeof(usf_hashmap));
	hashmap->lock = NULL; /* Non-blocking */
	usf_atmflagclr(&hashmap->spinlock, MEMORDER_RELEASE);
    hashmap->array = usf_calloc(capacity, sizeof(usf_hashentry));
	hashmap->size = 0;
	hashmap->capacity = capacity;

	return hashmap;
}

usf_hashmap *usf_newhmsz_mtx(u64 capacity) {
	/* Creates a new thread-blocking usf_hashmap initialized to 0 of given capacity.
	 * Returns the created hashmap, or NULL if a mutex cannot be created. */

	usf_hashmap *hashmap;
	hashmap = usf_newhmsz(capacity);
	hashmap->lock = usf_malloc(sizeof(usf_mutex));
	if (usf_mtxinit(hashmap->lock, MTXINIT_RECURSIVE) == THRD_ERROR) {
		usf_free(hashmap->lock);
		usf_free(hashmap->array);
		usf_free(hashmap);
		return NULL; /* mutex init failed */
	}

	return hashmap;
}

/* Common loop to find and access a hashmap element
 * _HASHMAP		reference to usf_hashmap *
 * _KEY			reference to the key
 * _HASHFUNC	hashing function
 * _ACCESS		statements to execute for each potential entry
 *
 * I_			full 64-bit hash
 * HASH_		hashed index into underlying array
 * ENTRY_		pointer to current hashmap entry being accessed
 * */
#define USF_HMACCESS(_HASHMAP, _KEY, _HASHFUNC, _ACCESS) \
	u64 I_, HASH_; \
	usf_hashentry *ENTRY_; \
	for (I_ = _HASHFUNC(_KEY);; I_ = usf_hash(I_)) { \
		HASH_ = I_ % _HASHMAP->capacity; \
		ENTRY_ = &_HASHMAP->array[HASH_]; \
		_ACCESS; \
	}

usf_hashmap *usf_strhmput(usf_hashmap *hashmap, const char *key, usf_data value) {
	/* Assigns a 64-bit usf_data value to this char *key.
	 * The key is hashed using usf_strhash.
	 * Returns the hashmap, or NULL on error. */

	if (hashmap == NULL || key == NULL) return NULL;

	if (hashmap->lock) usf_mtxlock(hashmap->lock); /* Thread-safe lock */
	else while (usf_atmflagtry(&hashmap->spinlock, MEMORDER_ACQ_REL)); /* Spinlock */

	if (hashmap->size + 1 > hashmap->capacity / USF_HASHMAP_RESIZE_MULTIPLIER)
		usf_internal_resizehm(hashmap, hashmap->capacity * USF_HASHMAP_RESIZE_MULTIPLIER);

	usf_hashentry *sentinel = NULL;
#define ACCESS \
	switch (ENTRY_->flag) { \
		case USF_HASHMAP_UNINITIALIZED: \
			if (sentinel) ENTRY_ = sentinel; \
			ENTRY_->key.p = usf_malloc(strlen(key) + 1); \
			strcpy(ENTRY_->key.p, key); \
			ENTRY_->flag = USF_HASHMAP_KEY_STRING; \
			hashmap->size++; \
			break; \
		\
		case USF_HASHMAP_KEY_STRING: \
			if (strcmp(ENTRY_->key.p, key)) \
				continue; /* Collision */ \
			break; \
		\
		case USF_HASHMAP_SENTINEL: \
			sentinel = ENTRY_; \
		default: /* Other key types */ \
			continue; \
	} \
	ENTRY_->value = value; \
	break;
	USF_HMACCESS(hashmap, key, usf_strhash, ACCESS);
#undef ACCESS

	if (hashmap->lock) usf_mtxunlock(hashmap->lock); /* Thread-safe unlock */
	else usf_atmflagclr(&hashmap->spinlock, MEMORDER_RELEASE); /* Spinunlock */

	return hashmap;
}

usf_data usf_strhmget(const usf_hashmap *hashmap, const char *key) {
	/* Returns the 64-bit usf_data value assigned to this char *key,
	 * or USFNULL (zero) if it is not accessible. */

	if (hashmap == NULL || key == NULL) return USFNULL;

	if (hashmap->lock) usf_mtxlock(hashmap->lock); /* Thread-safe lock */
	else while (usf_atmflagtry(&hashmap->spinlock, MEMORDER_ACQ_REL)); /* Spinlock */

	usf_data value = USFNULL;
#define ACCESS \
	if (ENTRY_->flag == USF_HASHMAP_UNINITIALIZED) break; \
	if (ENTRY_->flag == USF_HASHMAP_KEY_STRING && !strcmp(ENTRY_->key.p, key)) { \
		value = ENTRY_->value; \
		break; \
	}
	USF_HMACCESS(hashmap, key, usf_strhash, ACCESS);
#undef ACCESS

	if (hashmap->lock) usf_mtxunlock(hashmap->lock); /* Thread-safe unlock */
	else usf_atmflagclr(&hashmap->spinlock, MEMORDER_RELEASE); /* Spinunlock */

	return value;
}

usf_data usf_strhmdel(usf_hashmap *hashmap, const char *key) {
	/* Delete the 64-bit usf_data value assigned to this char *key.
	 * Returns the deleted value, or if it is not accessible */

	if (hashmap == NULL || key == NULL) return USFNULL;

	if (hashmap->lock) usf_mtxlock(hashmap->lock); /* Thread-safe lock */
	else while (usf_atmflagtry(&hashmap->spinlock, MEMORDER_ACQ_REL)); /* Spinlock */

	usf_data value = USFNULL;
#define ACCESS \
	if (ENTRY_->flag == USF_HASHMAP_UNINITIALIZED) break; \
	if (ENTRY_->flag == USF_HASHMAP_KEY_STRING && !strcmp(ENTRY_->key.p, key)) { \
		usf_free(ENTRY_->key.p); \
		ENTRY_->flag = USF_HASHMAP_SENTINEL; \
		value = ENTRY_->value; \
		hashmap->size--; \
		break; \
	}
	USF_HMACCESS(hashmap, key, usf_strhash, ACCESS);
#undef ACCESS

	if (hashmap->lock) usf_mtxunlock(hashmap->lock); /* Thread-safe unlock */ \
	else usf_atmflagclr(&hashmap->spinlock, MEMORDER_RELEASE); /* Spinunlock */

	return value;
}

usf_hashmap *usf_inthmput(usf_hashmap *hashmap, u64 key, usf_data value) {
	/* Assigns a 64-bit usf_data value to this u64 key.
	 * The key is hashed using usf_strhash.
	 * Returns the hashmap, or NULL on error. */

	if (hashmap == NULL) return NULL;

	if (hashmap->lock) usf_mtxlock(hashmap->lock); /* Thread-safe lock */
	else while (usf_atmflagtry(&hashmap->spinlock, MEMORDER_ACQ_REL)); /* Spinlock */

	if (hashmap->size + 1 > hashmap->capacity / USF_HASHMAP_RESIZE_MULTIPLIER)
		usf_internal_resizehm(hashmap, hashmap->capacity * USF_HASHMAP_RESIZE_MULTIPLIER);

	usf_hashentry *sentinel = NULL;
#define ACCESS \
	switch (ENTRY_->flag) { \
		case USF_HASHMAP_UNINITIALIZED: \
			if (sentinel) ENTRY_ = sentinel; \
			ENTRY_->key.u = key; \
			ENTRY_->flag = USF_HASHMAP_KEY_INTEGER; \
			hashmap->size++; \
			break; \
		\
		case USF_HASHMAP_KEY_INTEGER: \
			if (ENTRY_->key.u != key) \
				continue; /* Collision */ \
			break; \
		\
		case USF_HASHMAP_SENTINEL: \
			sentinel = ENTRY_; \
		default: /* Other key types */ \
			continue; \
	} \
	ENTRY_->value = value; \
	break;
	USF_HMACCESS(hashmap, key, usf_hash, ACCESS);
#undef ACCESS

	if (hashmap->lock) usf_mtxunlock(hashmap->lock); /* Thread-safe unlock */
	else usf_atmflagclr(&hashmap->spinlock, MEMORDER_RELEASE); /* Spinunlock */

	return hashmap;
}

usf_data usf_inthmget(const usf_hashmap *hashmap, u64 key) {
	/* Returns the 64-bit usf_data value assigned to this u64 key,
	 * or USFNULL (zero) if it is not accessible. */

	if (hashmap == NULL) return USFNULL;

	if (hashmap->lock) usf_mtxlock(hashmap->lock); /* Thread-safe lock */
	else while (usf_atmflagtry(&hashmap->spinlock, MEMORDER_ACQ_REL)); /* Spinlock */

	usf_data value = USFNULL;
#define ACCESS \
	if (ENTRY_->flag == USF_HASHMAP_UNINITIALIZED) break; \
	if (ENTRY_->flag == USF_HASHMAP_KEY_INTEGER && ENTRY_->key.u == key) { \
		value = ENTRY_->value; \
		break; \
	}
	USF_HMACCESS(hashmap, key, usf_hash, ACCESS);
#undef ACCESS

	if (hashmap->lock) usf_mtxunlock(hashmap->lock); /* Thread-safe unlock */
	else usf_atmflagclr(&hashmap->spinlock, MEMORDER_RELEASE); /* Spinunlock */

	return value;
}

usf_data usf_inthmdel(usf_hashmap *hashmap, u64 key) {
	/* Delete the 64-bit usf_data value assigned to this u64 *key.
	 * Returns the deleted value, or if it is not accessible */

	if (hashmap == NULL) return USFNULL;

	if (hashmap->lock) usf_mtxlock(hashmap->lock); /* Thread-safe lock */
	else while (usf_atmflagtry(&hashmap->spinlock, MEMORDER_ACQ_REL)); /* Spinlock */

	usf_data value = USFNULL;
#define ACCESS \
	if (ENTRY_->flag == USF_HASHMAP_UNINITIALIZED) break; \
	if (ENTRY_->flag == USF_HASHMAP_KEY_INTEGER && ENTRY_->key.u == key) { \
		ENTRY_->flag = USF_HASHMAP_SENTINEL; \
		value = ENTRY_->value; \
		hashmap->size--; \
		break; \
	}
	USF_HMACCESS(hashmap, key, usf_hash, ACCESS);
#undef ACCESS

	if (hashmap->lock) usf_mtxunlock(hashmap->lock); /* Thread-safe unlock */
	else usf_atmflagclr(&hashmap->spinlock, MEMORDER_RELEASE); /* Spinunlock */

	return value;
}
#undef USF_HMACCESS /* End of hashmap accessor functions */

void usf_hmiterbegin(usf_hashmap *hashmap, usf_hashiter *iter) {
	/* Initializes and begins a hashmap iterator for the given hashmap.
	 * After iteration has finished, usf_hmiterend must be called.
	 * (Note: an iterator must be ended by the same thread which initialized it) */

	usf_hmiterskim(hashmap, iter);
	if (hashmap->lock) usf_mtxlock(hashmap->lock); /* Thread-safe lock */
	else while (usf_atmflagtry(&hashmap->spinlock, MEMORDER_ACQ_REL)); /* Spinlock */
}

void usf_hmiterskim(usf_hashmap *hashmap, usf_hashiter *iter) {
	/* Initializes a fragile hashmap iterator for the given hashmap: using this iterator
	 * requires that no other processes modify the hashmap concurrently.
	 * However, usf_hmiterend does not need to be called afterwards. */

	iter->count = 0;
	iter->index = 0;
	iter->entry = NULL;
	iter->hashmap = hashmap;
}

usf_hashentry *usf_hmiternext(usf_hashiter *iter) {
	 /* Returns the next entry in the hashmap for this iterator, or NULL if there are no more. */

	usf_hashentry *entry;
	for (; iter->index < iter->hashmap->capacity && iter->count < iter->hashmap->size;) {
		entry = &iter->hashmap->array[iter->index++];
		if (entry->flag == USF_HASHMAP_UNINITIALIZED || entry->flag == USF_HASHMAP_SENTINEL)
			continue; /* Not an entry */
		iter->count++; /* Found */
		return iter->entry = entry;
	}
	return NULL;
}

void usf_hmiterend(usf_hashiter *iter) {
	/* This function must be called after hashmap iteration has concluded.
	 * (Note: an iterator must be ended by the same thread which initialized it) */

	if (iter->hashmap->lock) usf_mtxunlock(iter->hashmap->lock); /* Thread-safe unlock */
	else usf_atmflagclr(&iter->hashmap->spinlock, MEMORDER_RELEASE); /* Spinunlock */
}

void usf_internal_resizehm(usf_hashmap *hashmap, u64 size) {
	/* Resizes the underlying array of the provided hashmap to the requested size in hashentries.
	 * If size is smaller than or equal to the current size, this function has no effect.
	 * (Note: spinlocks aren't recursive; this function is not safe when using non-blocking hashmaps! */
	
	if (hashmap == NULL || hashmap->capacity >= size) return; /* Arguments have no effect */

	usf_hashmap *newhm;
	newhm = usf_newhmsz(size);

	usf_hashiter iter;
	for (usf_hmiterskim(hashmap, &iter); usf_hmiternext(&iter);) {
		switch (iter.entry->flag) {
			case USF_HASHMAP_UNINITIALIZED:
			case USF_HASHMAP_SENTINEL:
				continue; /* Skip */

			case USF_HASHMAP_KEY_INTEGER:
				usf_inthmput(newhm, iter.entry->key.u, iter.entry->value);
				break;

			case USF_HASHMAP_KEY_STRING:
				usf_strhmput(newhm, iter.entry->key.p, iter.entry->value);
				break;
		}
	}
	USF_SWAP(hashmap->array, newhm->array);
	USF_SWAP(hashmap->capacity, newhm->capacity);

	usf_freehm(newhm); /* Free temporary buffer */
}

void usf_hmclearfunc(usf_hashmap *hashmap, void (*freefunc)(void *)) {
	/* Clears (resets) a usf_hashmap and calls freefunc on its values.
	 * If freefunc is NULL, nothing is done to the hashmap values.
	 * If hashmap is NULL, this function has no effect. */

	if (hashmap == NULL) return;

	usf_hashiter iter; /* Thread-safe lock */
	for (usf_hmiterbegin(hashmap, &iter); usf_hmiternext(&iter);) {
		if (iter.entry->flag == USF_HASHMAP_KEY_STRING) usf_free(iter.entry->key.p);
		if (freefunc) freefunc(iter.entry->value.p);
		memset(iter.entry, 0, sizeof(usf_hashentry)); /* Clear */
	}
	hashmap->size = 0; /* Reset */
	usf_hmiterend(&iter); /* Thread-safe unlock */
}

void usf_hmclear(usf_hashmap *hashmap) {
	/* Clears (resets) a usf_hashmap without calling free on its values.
	 * If hashmap is NULL, this function has no effect. */

	usf_hmclearfunc(hashmap, NULL);
}

void usf_freehmfunc(usf_hashmap *hashmap, void (*freefunc)(void *)) {
	/* Frees a hashmap and calls freefunc on its values.
	 * If freefunc is NULL, nothing is done to the hashmap values.
	 * If hashmap is NULL, this function has no effect. */

	if (hashmap == NULL) return;

	usf_hashiter iter;
	for (usf_hmiterskim(hashmap, &iter); usf_hmiternext(&iter);) {
		if (iter.entry->flag == USF_HASHMAP_KEY_STRING) usf_free(iter.entry->key.p);
		if (freefunc) freefunc(iter.entry->value.p);
	}

	if (hashmap->lock) {
		usf_mtxdestroy(hashmap->lock);
		usf_free(hashmap->lock);
	}
	usf_free(hashmap->array);
	usf_free(hashmap);
}

void usf_freehm(usf_hashmap *hashmap) {
	/* Frees a hashmap without calling free on its values.
	 * If hashmap is NULL, this function has no effect. */

	usf_freehmfunc(hashmap, NULL);
}
