#include "usflist.h"

/* Common check to resize (grow) a list on access
 * _LIST		reference to the list
 * _INDEX		list index being accessed
 * _DATA		underlying list data
 *
 * RESIZESZ_	new list size, in _TYPEs
 * */

#define USF_LISTRESIZE(_LIST, _INDEX, _DATA) \
	u64 RESIZESZ_; \
	if (_INDEX >= _LIST->capacity) { /* Resize to either double old size, or enough to include i */ \
		RESIZESZ_ = USF_MAX(USF_LIST_RESIZE_MULTIPLIER * _LIST->capacity, _INDEX + 1); \
		\
		_LIST->array = usf_realloc(_LIST->array, RESIZESZ_ * sizeof(_DATA)); /* Realloc */ \
		memset(_LIST->array + _LIST->capacity, 0, (RESIZESZ_ - _LIST->capacity) * sizeof(_DATA)); \
		\
		_LIST->size = _INDEX + 1; \
		_LIST->capacity = RESIZESZ_; \
	} else _LIST->size = USF_MAX(_LIST->size, _INDEX + 1); /* No array resize, but list growth */

/* Generic list implementation
 * _TYPE		underlying list type
 * _NAME		list name suffix (e.g. f32 -> usf_listf32)
 * */

#define USF_LISTIMPL(_TYPE, _NAME) \
	usf_list##_NAME *usf_newlist##_NAME(void) { \
		/* Wrapper for creating default-sized non thread-safe lists. */ \
		\
		return usf_newlist##_NAME##sz(USF_LIST_DEFAULTSIZE); \
	} \
	\
	usf_list##_NAME *usf_newlist##_NAME##_ts(void) { \
		/* Wrapper for creating default-sized thread-safe lists. */ \
		\
		return usf_newlist##_NAME##sz_ts(USF_LIST_DEFAULTSIZE); \
	} \
	\
	usf_list##_NAME *usf_newlist##_NAME##sz(u64 capacity) { \
		/* Creates a new non thread-safe memory-contiguous list, initialized to 0 of given capacity.
		 * Returns the created list. */ \
		\
		usf_list##_NAME *list; \
		list = usf_malloc(sizeof(usf_list##_NAME)); \
		list->lock = NULL; \
		list->array = usf_calloc(capacity, sizeof(_TYPE)); \
		list->size = 0; \
		list->capacity = capacity; \
		\
		return list; \
	} \
	\
	usf_list##_NAME *usf_newlist##_NAME##sz_ts(u64 capacity) { \
		/* Creates a new thread-safe memory-contiguous list, initialized to 0 of given capacity.
		 * Returns the created list, or NULL if a mutex cannot be created. */ \
		\
		usf_list##_NAME *list; \
		list = usf_malloc(sizeof(usf_list##_NAME)); \
		list->lock = usf_malloc(sizeof(usf_mutex)); \
		if (usf_mtxinit(list->lock, MTXINIT_RECURSIVE)) { \
			usf_free(list->lock); \
			usf_free(list); \
			return NULL; /* mutex init failed */ \
		} \
		list->array = usf_calloc(capacity, sizeof(_TYPE)); \
		list->size = 0; \
		list->capacity = capacity; \
		\
		return list; \
	} \
	\
	usf_list##_NAME *usf_list##_NAME##set(usf_list##_NAME *list, u64 i, _TYPE data) { \
		/* Sets the given data at index i in the list, resizing and initializing to 0 if necessary.
		 * Returns the list, or NULL if an error occurred. */ \
		\
		if (list == NULL) return NULL; \
		if (list->lock) usf_mtxlock(list->lock); /* Thread-safe lock */ \
		\
		USF_LISTRESIZE(list, i, data); \
		list->array[i] = data; \
		\
		if (list->lock) usf_mtxunlock(list->lock); /* Thread-safe unlock */ \
		return list; \
	} \
	\
	usf_list##_NAME *usf_list##_NAME##ins(usf_list##_NAME *list, u64 i, _TYPE data) { \
		/* Inserts the given data at index i in the list, resizing and initializing to 0 if necessary.
		 * The element will be inserted before the element that currently has that index, if it exists.
		 * Returns the list, or NULL if an error occurred. */ \
		\
		if (list == NULL) return NULL; \
		if (list->lock) usf_mtxlock(list->lock); /* Thread-safe lock */ \
		\
		USF_LISTRESIZE(list, USF_MAX(list->size, i), data); \
		if (i < list->size) \
			memmove(&list->array[i + 1], &list->array[i], (list->size - (i + 1)) * sizeof(_TYPE)); \
		list->array[i] = data; \
		\
		if (list->lock) usf_mtxunlock(list->lock); /* Thread-safe unlock */ \
		return list; \
	} \
	\
	usf_list##_NAME *usf_list##_NAME##add(usf_list##_NAME *list, _TYPE data) { \
		/* Appends the given data at index list->size in the list, resizing and initializing to 0 if necessary.
		 * Effectively a wrapper for usf_list##_NAMEset(list, list->size, data).
		 * Returns the list, or NULL if an error occurred. */ \
		\
		if (list == NULL) return NULL; \
		if (list->lock) usf_mtxlock(list->lock); /* Thread-safe lock */ \
		\
		u64 i; \
		USF_LISTRESIZE(list, (i = list->size), data); \
		list->array[i] = data; \
		\
		if (list->lock) usf_mtxunlock(list->lock); /* Thread-safe unlock */ \
		return list; \
	} \
	\
	_TYPE usf_list##_NAME##get(const usf_list##_NAME *list, u64 i) { \
		/* Returns the data at index i in the given list, or zero if it is inaccessible. */ \
		\
		if (list == NULL) return (_TYPE) {0}; \
		if (list->lock) usf_mtxlock(list->lock); /* Thread-safe lock */ \
		\
		_TYPE data; \
		if (i >= list->size) data = (_TYPE) {0}; \
		else data = list->array[i]; \
		\
		if (list->lock) usf_mtxunlock(list->lock); /* Thread-safe unlock */ \
		return data; \
	} \
	\
	_TYPE usf_list##_NAME##del(usf_list##_NAME *list, u64 i) { \
		/* Deletes the element at index i in the given list, while maintaining contiguity.
		 * Not the same as setting to 0; effectively offsetting everything after i by 1 backwards.
		 * Returns the deleted value, or zero if it is inaccessible. */ \
		\
		if (list == NULL) return (_TYPE) {0}; \
		if (list->lock) usf_mtxlock(list->lock); /* Thread-safe lock */ \
		\
		_TYPE data; \
		if (i >= list->size) data = (_TYPE) {0}; \
		else { \
			data = list->array[i]; \
			memmove(&list->array[i], &list->array[i + 1], (list->size - (i + 1)) * sizeof(_TYPE)); \
			list->size--; \
		} \
		\
		if (list->lock) usf_mtxunlock(list->lock); /* Thread-safe unlock */ \
		return data; \
	} \
	\
	void usf_freelist##_NAME##func(usf_list##_NAME *list, void (*freefunc)(_TYPE)) { \
		/* Frees a list and calls freefunc on its values.
		 * If freefunc is NULL, nothing is done to the values.
		 * If list is NULL, this function has no effect. */ \
		\
		if (list == NULL) return; \
		\
		u64 i; \
		if (freefunc) for (i = 0; i < list->size; i++) \
			freefunc(list->array[i]); /* Free value */ \
		\
		usf_free(list->array); \
		if (list->lock) { \
			usf_mtxdestroy(list->lock); \
			usf_free(list->lock); \
		} \
		usf_free(list); \
	} \
	\
	void usf_freelist##_NAME(usf_list##_NAME *list) { \
		/* Frees a list without freeing its values.
		 * If list is NULL, this function has no effect. */ \
		\
		usf_freelist##_NAME##func(list, NULL); \
	}
USF_LISTIMPL(i8, i8)
USF_LISTIMPL(i16, i16)
USF_LISTIMPL(i32, i32)
USF_LISTIMPL(i64, i64)
USF_LISTIMPL(u8, u8)
USF_LISTIMPL(u16, u16)
USF_LISTIMPL(u32, u32)
USF_LISTIMPL(u64, u64)
USF_LISTIMPL(f32, f32)
USF_LISTIMPL(f64, f64)
USF_LISTIMPL(void *, ptr)
USF_LISTIMPL(usf_data, )
#undef USF_LISTIMPL
#undef USF_LISTRESIZE
