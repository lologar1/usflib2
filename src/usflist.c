#include "usflist.h"

/* Common check to resize (grow) a list on access
 * _LIST		reference to the list
 * _INDEX		list index being accessed
 * _DATA		underlying list data
 *
 * _RESIZESZ	new list size, in _TYPEs
 * */

#define _USF_LISTRESIZE(_LIST, _INDEX, _DATA) \
	u64 _RESIZESZ; \
	if (_INDEX >= _LIST->capacity) { /* Resize to either double old size, or enough to include i */ \
		_RESIZESZ = USF_MAX(USF_LIST_RESIZE_MULTIPLIER * _LIST->capacity, _INDEX + 1); \
		\
		_LIST->array = usf_realloc(_LIST->array, _RESIZESZ * sizeof(_DATA)); /* Realloc */ \
		memset(_LIST->array + _LIST->capacity, 0, (_RESIZESZ - _LIST->capacity) * sizeof(_DATA)); \
		\
		_LIST->size = _INDEX + 1; \
		_LIST->capacity = _RESIZESZ; \
	} else _LIST->size = USF_MAX(_LIST->size, _INDEX + 1); /* No array resize, but list growth */

/* Generic list implementation
 * _TYPE		underlying list type
 * _NAME		list name suffix (e.g. f32 -> usf_listf32)
 * */

#define _USF_LISTIMPL(_TYPE, _NAME) \
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
		list->lock = usf_malloc(sizeof(pthread_mutex_t)); \
		if (pthread_mutex_init(list->lock, NULL)) { /* Default attributes */ \
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
		if (list->lock) pthread_mutex_lock(list->lock); /* Thread-safe lock */ \
		\
		_USF_LISTRESIZE(list, i, data); \
		list->array[i] = data; \
		\
		if (list->lock) pthread_mutex_unlock(list->lock); /* Thread-safe unlock */ \
		return list; \
	} \
	\
	usf_list##_NAME *usf_list##_NAME##ins(usf_list##_NAME *list, u64 i, _TYPE data) { \
		/* Inserts the given data at index i in the list, resizing and initializing to 0 if necessary.
		 * The element will be inserted before the element that currently has that index, if it exists.
		 * Returns the list, or NULL if an error occurred. */ \
		\
		if (list == NULL) return NULL; \
		if (list->lock) pthread_mutex_lock(list->lock); /* Thread-safe lock */ \
		\
		_USF_LISTRESIZE(list, USF_MAX(list->size, i), data); \
		if (i < list->size) \
			memmove(&list->array[i + 1], &list->array[i], (list->size - (i + 1)) * sizeof(_TYPE)); \
		list->array[i] = data; \
		\
		if (list->lock) pthread_mutex_unlock(list->lock); /* Thread-safe unlock */ \
		return list; \
	} \
	\
	usf_list##_NAME *usf_list##_NAME##add(usf_list##_NAME *list, _TYPE data) { \
		/* Appends the given data at index list->size in the list, resizing and initializing to 0 if necessary.
		 * Effectively a wrapper for usf_list##_NAMEset(list, list->size, data).
		 * Returns the list, or NULL if an error occurred. */ \
		\
		if (list == NULL) return NULL; \
		if (list->lock) pthread_mutex_lock(list->lock); /* Thread-safe lock */ \
		\
		u64 i; \
		_USF_LISTRESIZE(list, (i = list->size), data); \
		list->array[i] = data; \
		\
		if (list->lock) pthread_mutex_unlock(list->lock); /* Thread-safe unlock */ \
		return list; \
	} \
	\
	_TYPE usf_list##_NAME##get(usf_list##_NAME *list, u64 i) { \
		/* Returns the data at index i in the given list, or zero if it is inaccessible. */ \
		\
		if (list == NULL) return (_TYPE) {0}; \
		if (list->lock) pthread_mutex_lock(list->lock); /* Thread-safe lock */ \
		\
		_TYPE data; \
		if (i >= list->size) data = (_TYPE) {0}; \
		else data = list->array[i]; \
		\
		if (list->lock) pthread_mutex_unlock(list->lock); /* Thread-safe unlock */ \
		return data; \
	} \
	\
	_TYPE usf_list##_NAME##del(usf_list##_NAME *list, u64 i) { \
		/* Deletes the element at index i in the given list, while maintaining contiguity.
		 * Not the same as setting to 0; effectively offsetting everything after i by 1 backwards.
		 * Returns the deleted value, or zero if it is inaccessible. */ \
		\
		if (list == NULL) return (_TYPE) {0}; \
		if (list->lock) pthread_mutex_lock(list->lock); /* Thread-safe lock */ \
		\
		_TYPE data; \
		if (i >= list->size) data = (_TYPE) {0}; \
		else { \
			data = list->array[i]; \
			memmove(&list->array[i], &list->array[i + 1], (list->size - (i + 1)) * sizeof(_TYPE)); \
			list->size--; \
		} \
		\
		if (list->lock) pthread_mutex_unlock(list->lock); /* Thread-safe unlock */ \
		return data; \
	} \
	\
	void usf_freelist##_NAME(usf_list##_NAME *list) { \
		/* Frees a list without calling usf_free on its values.
		 * If list is NULL, this function has no effect. */ \
		\
		if (list == NULL) return; \
		\
		usf_free(list->array); \
		\
		if (list->lock) { \
			pthread_mutex_destroy(list->lock); \
			usf_free(list->lock); \
		} \
		usf_free(list); \
	}
_USF_LISTIMPL(i8, i8)
_USF_LISTIMPL(i16, i16)
_USF_LISTIMPL(i32, i32)
_USF_LISTIMPL(i64, i64)
_USF_LISTIMPL(u8, u8)
_USF_LISTIMPL(u16, u16)
_USF_LISTIMPL(u32, u32)
_USF_LISTIMPL(u64, u64)
_USF_LISTIMPL(f32, f32)
_USF_LISTIMPL(f64, f64)
_USF_LISTIMPL(void *, ptr)
_USF_LISTIMPL(usf_data, )
#undef _USF_LISTIMPL
#undef _USF_LISTRESIZE
