#ifndef USFLIST_H
#define USFLIST_H

#include <string.h>
#include <pthread.h>
#include "usfstd.h"
#include "usfdata.h"
#include "usfmath.h"

#define USF_LIST_DEFAULTSIZE 16
#define USF_LIST_RESIZE_MULTIPLIER 2

/* Generic list declaration for multiple possible underlying types */
#define _USF_LISTDECL(_TYPE, _NAME) \
	typedef struct usf_list##_NAME { \
		pthread_mutex_t *lock; \
		_TYPE *array; \
		u64 size; \
		u64 capacity; \
	} usf_list##_NAME; \
	\
	usf_list##_NAME *usf_newlist##_NAME(void); \
	usf_list##_NAME *usf_newlist##_NAME##_ts(void); \
	usf_list##_NAME *usf_newlist##_NAME##sz(u64 capacity); \
	usf_list##_NAME *usf_newlist##_NAME##sz_ts(u64 capacity); \
	usf_list##_NAME *usf_list##_NAME##set(usf_list##_NAME *list, u64 i, _TYPE data); \
	usf_list##_NAME *usf_list##_NAME##ins(usf_list##_NAME *list, u64 i, _TYPE data); \
	usf_list##_NAME *usf_list##_NAME##add(usf_list##_NAME *list, _TYPE data); \
	_TYPE usf_list##_NAME##get(const usf_list##_NAME *list, u64 i); \
	_TYPE usf_list##_NAME##del(usf_list##_NAME *list, u64 i); \
	void usf_freelist##_NAME(usf_list##_NAME *list);
_USF_LISTDECL(i8, i8)
_USF_LISTDECL(i16, i16)
_USF_LISTDECL(i32, i32)
_USF_LISTDECL(i64, i64)
_USF_LISTDECL(u8, u8)
_USF_LISTDECL(u16, u16)
_USF_LISTDECL(u32, u32)
_USF_LISTDECL(u64, u64)
_USF_LISTDECL(f32, f32)
_USF_LISTDECL(f64, f64)
_USF_LISTDECL(void *, ptr)
_USF_LISTDECL(usf_data, )
#undef _USF_LISTDECL

#endif
