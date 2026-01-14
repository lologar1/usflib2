#include "usfskiplist.h"

usf_skiplist *usf_newsk(void) {
	/* Creates a new non thread-safe skiplist, initialized to 0.
	 * Returns the created skiplist. */

	usf_skiplist *skiplist;
	skiplist = usf_calloc(1, sizeof(usf_skiplist));

	return skiplist;
}

usf_skiplist *usf_newsk_ts(void) {
	/* Creates a new thread-safe skiplist, initialized to 0.
	 * Returns the created skiplist, or NULL if a mutex cannot be created. */

	usf_skiplist *skiplist;
	skiplist = usf_calloc(1, sizeof(usf_skiplist));
	skiplist->lock = usf_malloc(sizeof(pthread_mutex_t));
	if (pthread_mutex_init(skiplist->lock, NULL)) { /* Default attributes */
		usf_free(skiplist);
		return NULL; /* mutex init failed */
	}

	return skiplist;
}

usf_skiplist *usf_skset(usf_skiplist *skiplist, u64 i, usf_data data) {
	/* Inserts the given data at virtual index i in the skiplist.
	 * Returns the skiplist, or NULL if an error occurred. */

	if (skiplist == NULL) return NULL;
	if (skiplist->lock) pthread_mutex_lock(skiplist->lock); /* Thread-safe lock */

	i32 level;
	usf_skipnode **skiplinks[USF_SKIPLIST_FRAMESIZE], **skipframe, *node;
	for (skipframe = skiplist->base, level = USF_SKIPLIST_FRAMESIZE - 1; level >= 0; level--) {
		while ((node = skipframe[level])) {
			if (node->index > i) break; /* Overshot */
			if (node->index == i) { /* Found; only modify node */
				node->data = data;
				if (skiplist->lock) pthread_mutex_unlock(skiplist->lock); /* Thread-safe unlock */
				return skiplist;
			}
			skipframe = node->nextnodes; /* Skip along */
		}
		skiplinks[level] = &skipframe[level];
	}

	node = usf_calloc(1, sizeof(usf_skipnode));
	node->data = data; node->index = i;
	for (level = 0; level < USF_SKIPLIST_FRAMESIZE; level++) {
		node->nextnodes[level] = *skiplinks[level]; /* Link this with next */
		*skiplinks[level] = node; /* Link prev with this; this why we we keep extra indirection */

		if (rand() & 1) break; /* Probabilistic upkeep */
	}
	skiplist->size++;

	if (skiplist->lock) pthread_mutex_unlock(skiplist->lock); /* Thread-safe unlock */
	return skiplist;
}

usf_data usf_skget(usf_skiplist *skiplist, u64 i) {
	/* Returns the data at virtual index i in the given skiplist,
	 * or USFNULL (zero) if it is inaccessible. */

	if (skiplist == NULL) return USFNULL;
	if (skiplist->lock) pthread_mutex_lock(skiplist->lock); /* Thread-safe lock */

	int32_t level;
	usf_skipnode **skipframe, *node;
	for (skipframe = skiplist->base, level = USF_SKIPLIST_FRAMESIZE - 1; level >= 0; level--) {
		while ((node = skipframe[level])) {
			if (node->index > i) break; /* Overshot */
			if (node->index == i) { /* Found */
				if (skiplist->lock) pthread_mutex_unlock(skiplist->lock); /* Thread-safe unlock */
				return node->data;
			}
			skipframe = node->nextnodes; /* Skip along */
		}
	}

	if (skiplist->lock) pthread_mutex_unlock(skiplist->lock); /* Thread-safe unlock */
	return USFNULL;
}

usf_data usf_skdel(usf_skiplist *skiplist, u64 i) {
	/* Deletes the 64-bit usf_data at virtual index i in the given skiplist.
	 * Returns the deleted value, or USFNULL (zero) if it is not accessible. */

	if (skiplist == NULL) return USFNULL;
	if (skiplist->lock) pthread_mutex_lock(skiplist->lock); /* Thread-safe lock */

	int32_t level;
	usf_skipnode **skipframe, *node;
	for (skipframe = skiplist->base, level = USF_SKIPLIST_FRAMESIZE - 1; level >= 0; level--) {
		while ((node = skipframe[level])) {
			if (node->index > i) break; /* Overshot */
			if (node->index == i) { /* Unlink */
				skipframe[level] = node->nextnodes[level];
				break;
			}
			skipframe = node->nextnodes; /* Skip along */
		}
	}

	usf_data data;
	if (node && node->index == i) { /* Found */
		data = node->data;
		usf_free(node);
	} else data = USFNULL;

	if (skiplist->lock) pthread_mutex_unlock(skiplist->lock); /* Thread-safe unlock */
	return data;
}

void usf_freesk(usf_skiplist *skiplist) {
	/* Frees a skiplist without calling usf_free on its values.
	 * If skiplist is NULL, this function has no effect. */

	if (skiplist == NULL) return;

	usf_skipnode *node, *next;
	for (node = skiplist->base[0]; node; node = next) {
		next = node->nextnodes[0];
		usf_free(node);
	}

	if (skiplist->lock) {
		pthread_mutex_destroy(skiplist->lock);
		usf_free(skiplist->lock);
	}
	usf_free(skiplist);
}

void usf_freeskptr(usf_skiplist *skiplist) {
	/* Frees a skiplist and calls usf_free on its values.
	 * If skiplist is NULL, this function has no effect. */

	if (skiplist == NULL) return;

	usf_skipnode *node, *next;
	for (node = skiplist->base[0]; node; node = next) {
		next = node->nextnodes[0];
		usf_free(node->data.p);
		usf_free(node);
	}

	if (skiplist->lock) {
		pthread_mutex_destroy(skiplist->lock);
		usf_free(skiplist->lock);
	}
	usf_free(skiplist);
}
