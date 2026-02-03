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
	if (usf_mtxinit(skiplist->lock, MTXINIT_PLAIN)) {
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
 * _LEVEL		current skiplevel
 * _SKIPFRAME	previous skipnode's pointers to next nodes
 * _NODE		current skipnode being accessed
 * */

#define _USF_SKACCESS(_SKIPLIST, _INDEX, _ACCESS, _LEVELSHIFT) \
	i32 _LEVEL; \
	usf_skipnode **_SKIPFRAME, *_NODE; \
	for (_SKIPFRAME = _SKIPLIST->base, _LEVEL = USF_SKIPLIST_FRAMESIZE - 1; _LEVEL >= 0; _LEVEL--) { \
		while ((_NODE = _SKIPFRAME[_LEVEL])) { \
			if (_NODE->index > _INDEX) break; /* Overshot */ \
			if (_NODE->index == _INDEX) { /* Found */ \
				_ACCESS(_SKIPLIST, _INDEX); \
			} \
			_SKIPFRAME = _NODE->nextnodes; /* Skip along */ \
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
#define _ACCESS(_SKIPLIST, _INDEX) \
	_NODE->data = data; \
	if (_SKIPLIST->lock) usf_mtxunlock(_SKIPLIST->lock); /* Thread-safe unlock */ \
	return _SKIPLIST;
	_USF_SKACCESS(skiplist, i, _ACCESS, skiplinks[_LEVEL] = &_SKIPFRAME[_LEVEL]);
#undef _ACCESS

	_NODE = usf_calloc(1, sizeof(usf_skipnode));
	_NODE->data = data; _NODE->index = i;
	for (_LEVEL = 0; _LEVEL < USF_SKIPLIST_FRAMESIZE; _LEVEL++) {
		_NODE->nextnodes[_LEVEL] = *skiplinks[_LEVEL]; /* Link this with next */
		*skiplinks[_LEVEL] = _NODE; /* Link prev with this; this why we we keep extra indirection */

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

#define _ACCESS(_SKIPLIST, _INDEX) \
	if (_SKIPLIST->lock) usf_mtxunlock(_SKIPLIST->lock); /* Thread-safe unlock */ \
	return _NODE->data;
	/* Note: _USF_SKACCESS discards const qualifier as it builds mutable structures for
	 * use in other functions (i.e. skipnode linking). That warning is disabled here
	 * since usf_skget only queries these for traversal. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
	_USF_SKACCESS(skiplist, i, _ACCESS, (void) 0);
#pragma GCC diagnostic pop
#undef _ACCESS

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

#define _ACCESS(_SKIPLIST, _INDEX) \
	_SKIPFRAME[_LEVEL] = _NODE->nextnodes[_LEVEL]; /* Unlink */ \
	break;
	_USF_SKACCESS(skiplist, i, _ACCESS, (void) 0);
#undef _ACCESS

	usf_data data;
	if (_NODE && _NODE->index == i) { /* Found */
		data = _NODE->data;
		usf_free(_NODE);
	} else data = USFNULL;

	if (skiplist->lock) usf_mtxunlock(skiplist->lock); /* Thread-safe unlock */
	return data;
}
#undef _USF_SKACCESS

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
