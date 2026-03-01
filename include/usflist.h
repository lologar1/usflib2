#ifndef USFLIST_H
#define USFLIST_H

#include <string.h>
#include "usfstd.h"
#include "usfdata.h"
#include "usfmath.h"
#include "usfthread.h"

#define USF_LIST_DEFAULTSIZE 16
#define USF_LIST_RESIZE_MULTIPLIER 2

/* Generic list declaration for multiple possible underlying types */
#define USF_LISTDECL(_TYPE, _NAME) \
	typedef struct usf_list##_NAME { \
		usf_mutex *lock; \
		_TYPE *array; \
		u64 size; \
		u64 capacity; \
	} usf_list##_NAME; \
	\
	usf_list##_NAME *usf_newlist##_NAME(void); \
	usf_list##_NAME *usf_newlist##_NAME##_ts(void); \
	usf_list##_NAME *usf_newlist##_NAME##sz(u64 capacity); \
	usf_list##_NAME *usf_newlist##_NAME##sz_ts(u64 capacity); \
	\
	usf_list##_NAME *usf_list##_NAME##set(usf_list##_NAME *list, u64 i, _TYPE data);	/* Thread-safe */ \
	usf_list##_NAME *usf_list##_NAME##ins(usf_list##_NAME *list, u64 i, _TYPE data);	/* Thread-safe */ \
	usf_list##_NAME *usf_list##_NAME##add(usf_list##_NAME *list, _TYPE data);			/* Thread-safe */ \
	_TYPE usf_list##_NAME##get(const usf_list##_NAME *list, u64 i);						/* Thread-safe */ \
	_TYPE usf_list##_NAME##del(usf_list##_NAME *list, u64 i);							/* Thread-safe */ \
	\
	void usf_freelist##_NAME##func(usf_list##_NAME *list, void (*freefunc)(_TYPE)); \
	void usf_freelist##_NAME(usf_list##_NAME *list);
USF_LISTDECL(i8, i8)
USF_LISTDECL(i16, i16)
USF_LISTDECL(i32, i32)
USF_LISTDECL(i64, i64)
USF_LISTDECL(u8, u8)
USF_LISTDECL(u16, u16)
USF_LISTDECL(u32, u32)
USF_LISTDECL(u64, u64)
USF_LISTDECL(f32, f32)
USF_LISTDECL(f64, f64)
USF_LISTDECL(void *, ptr)
USF_LISTDECL(usf_data, )
#undef USF_LISTDECL

#endif
