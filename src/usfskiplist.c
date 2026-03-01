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
	skiplist->lock = usf_malloc(sizeof(usf_mutex));
	if (usf_mtxinit(skiplist->lock, MTXINIT_RECURSIVE)) {
		usf_free(skiplist->lock);
		usf_free(skiplist);
		return NULL; /* mutex init failed */
	}

	return skiplist;
}

/* Common loop to find and access a skiplist element
 * _SKIPLIST	reference to usf_skiplist *
 * _INDEX		virtual skiplist index being accessed
 * _ACCESS		statements to execute when a match is found
 * _LEVELSHIFT	statement to execute on skiplevel shift
 *
 * LEVEL_		current skiplevel
 * SKIPFRAME_	previous skipnode's pointers to next nodes
 * NODE_		current skipnode being accessed
 * */

#define USF_SKACCESS(_SKIPLIST, _INDEX, _ACCESS, _LEVELSHIFT) \
	i32 LEVEL_; \
	usf_skipnode **SKIPFRAME_, *NODE_; \
	for (SKIPFRAME_ = _SKIPLIST->base, LEVEL_ = USF_SKIPLIST_FRAMESIZE - 1; LEVEL_ >= 0; LEVEL_--) { \
		while ((NODE_ = SKIPFRAME_[LEVEL_])) { \
			if (NODE_->index > _INDEX) break; /* Overshot */ \
			if (NODE_->index == _INDEX) { /* Found */ \
				_ACCESS(_SKIPLIST, _INDEX); \
			} \
			SKIPFRAME_ = NODE_->nextnodes; /* Skip along */ \
		} \
		_LEVELSHIFT; \
	}

usf_skiplist *usf_skset(usf_skiplist *skiplist, u64 i, usf_data data) {
	/* This function is thread-safe when operating on thread-safe skiplists.
	 *
	 * Sets the given data at virtual index i in the skiplist.
	 * Returns the skiplist, or NULL if an error occurred. */

	if (skiplist == NULL) return NULL;
	if (skiplist->lock) usf_mtxlock(skiplist->lock); /* Thread-safe lock */

	usf_skipnode **skiplinks[USF_SKIPLIST_FRAMESIZE];
#define ACCESS(_SKIPLIST, _INDEX) \
	NODE_->data = data; \
	if (_SKIPLIST->lock) usf_mtxunlock(_SKIPLIST->lock); /* Thread-safe unlock */ \
	return _SKIPLIST;
	USF_SKACCESS(skiplist, i, ACCESS, skiplinks[LEVEL_] = &SKIPFRAME_[LEVEL_]);
#undef ACCESS

	NODE_ = usf_calloc(1, sizeof(usf_skipnode));
	NODE_->data = data; NODE_->index = i;
	for (LEVEL_ = 0; LEVEL_ < USF_SKIPLIST_FRAMESIZE; LEVEL_++) {
		NODE_->nextnodes[LEVEL_] = *skiplinks[LEVEL_]; /* Link this with next */
		*skiplinks[LEVEL_] = NODE_; /* Link prev with this; this why we we keep extra indirection */

		if (rand() & 1) break; /* Probabilistic upkeep */
	}
	skiplist->size++;

	if (skiplist->lock) usf_mtxunlock(skiplist->lock); /* Thread-safe unlock */
	return skiplist;
}

usf_data usf_skget(const usf_skiplist *skiplist, u64 i) {
	/* This function is thread-safe when operating on thread-safe skiplists.
	 *
	 * Returns the data at virtual index i in the given skiplist,
	 * or USFNULL (zero) if it is inaccessible. */

	if (skiplist == NULL) return USFNULL;
	if (skiplist->lock) usf_mtxlock(skiplist->lock); /* Thread-safe lock */

#define ACCESS(_SKIPLIST, _INDEX) \
	if (_SKIPLIST->lock) usf_mtxunlock(_SKIPLIST->lock); /* Thread-safe unlock */ \
	return NODE_->data;
	/* Note: USF_SKACCESS discards const qualifier as it builds mutable structures for
	 * use in other functions (i.e. skipnode linking). That warning is disabled here
	 * since usf_skget only queries these for traversal. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
	USF_SKACCESS(skiplist, i, ACCESS, (void) 0);
#pragma GCC diagnostic pop
#undef ACCESS

	if (skiplist->lock) usf_mtxunlock(skiplist->lock); /* Thread-safe unlock */
	return USFNULL;
}

usf_data usf_skdel(usf_skiplist *skiplist, u64 i) {
	/* This function is thread-safe when operating on thread-safe skiplists.
	 *
	 * Deletes the 64-bit usf_data at virtual index i in the given skiplist.
	 * Returns the deleted value, or USFNULL (zero) if it is not accessible. */

	if (skiplist == NULL) return USFNULL;
	if (skiplist->lock) usf_mtxlock(skiplist->lock); /* Thread-safe lock */

#define ACCESS(_SKIPLIST, _INDEX) \
	SKIPFRAME_[LEVEL_] = NODE_->nextnodes[LEVEL_]; /* Unlink */ \
	break;
	USF_SKACCESS(skiplist, i, ACCESS, (void) 0);
#undef ACCESS

	usf_data data;
	if (NODE_ && NODE_->index == i) { /* Found */
		data = NODE_->data;
		usf_free(NODE_);
	} else data = USFNULL;

	if (skiplist->lock) usf_mtxunlock(skiplist->lock); /* Thread-safe unlock */
	return data;
}
#undef USF_SKACCESS

void usf_freeskfunc(usf_skiplist *skiplist, void (*freefunc)(void *)) {
	/* Frees a skiplist and calls freefunc on its values.
	 * If freefunc is NULL, nothing is done on the skiplist values.
	 * If skiplist is NULL, this function has no effect. */
	
	if (skiplist == NULL) return;

	usf_skipnode *node, *next;
	for (node = skiplist->base[0]; node; node = next) {
		next = node->nextnodes[0];
		if (freefunc) freefunc(node->data.p);
		usf_free(node);
	}

	if (skiplist->lock) {
		usf_mtxdestroy(skiplist->lock);
		usf_free(skiplist->lock);
	}
	usf_free(skiplist);
}

void usf_freeskptr(usf_skiplist *skiplist) {
	/* Frees a skiplist and calls usf_free on its values.
	 * If skiplist is NULL, this function has no effect. */

	usf_freeskfunc(skiplist, usf_free);
}

void usf_freesk(usf_skiplist *skiplist) {
	/* Frees a skiplist without freeing its values.
	 * If skiplist is NULL, this function has no effect. */

	usf_freeskfunc(skiplist, NULL);
}
