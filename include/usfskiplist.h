#ifndef USFSKIPLIST_H
#define USFSKIPLIST_H

#include <string.h>
#include "usfdata.h"
#include "usfthread.h"

#define USF_SKIPLIST_FRAMESIZE 24

typedef struct usf_skipnode {
	struct usf_skipnode *nextnodes[USF_SKIPLIST_FRAMESIZE];
	usf_data data;
	u64 index;
} usf_skipnode;

typedef struct usf_skiplist {
	usf_mutex *lock;
	usf_skipnode *base[USF_SKIPLIST_FRAMESIZE];
	u64 size;
} usf_skiplist;

usf_skiplist *usf_newsk(void);
usf_skiplist *usf_newsk_ts(void);

usf_skiplist *usf_skset(usf_skiplist *skiplist, u64 i, usf_data data);	/* Thread-safe */
usf_data usf_skget(const usf_skiplist *skiplist, u64 data);				/* Thread-safe */
usf_data usf_skdel(usf_skiplist *skiplist, u64 data);					/* Thread-safe */

void usf_freeskfunc(usf_skiplist *skiplist, void (*freefunc)(void *));
void usf_freeskptr(usf_skiplist *skiplist);
void usf_freesk(usf_skiplist *skiplist);

#endif
