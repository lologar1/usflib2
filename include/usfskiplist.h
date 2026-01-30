#ifndef USFSKIPLIST_H
#define USFSKIPLIST_H

#include <string.h>
#include <pthread.h>
#include "usfdata.h"

#define USF_SKIPLIST_FRAMESIZE 24

typedef struct usf_skipnode {
	struct usf_skipnode *nextnodes[USF_SKIPLIST_FRAMESIZE];
	usf_data data;
	u64 index;
} usf_skipnode;

typedef struct usf_skiplist {
	pthread_mutex_t *lock;
	usf_skipnode *base[USF_SKIPLIST_FRAMESIZE];
	u64 size;
} usf_skiplist;

usf_skiplist *usf_newsk(void);
usf_skiplist *usf_newsk_ts(void);
usf_skiplist *usf_skset(usf_skiplist *skiplist, u64 i, usf_data data);
usf_data usf_skget(usf_skiplist *skiplist, u64 data);
usf_data usf_skdel(usf_skiplist *skiplist, u64 data);
void usf_freeskfunc(usf_skiplist *skiplist, void (*freefunc)(void *));
void usf_freeskptr(usf_skiplist *skiplist);
void usf_freesk(usf_skiplist *skiplist);

#endif
