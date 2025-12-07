#ifndef USFSKIPLIST_H
#define USFSKIPLIST_H

#include <stdlib.h>
#include <string.h>
#include "usfdata.h"

#define USF_SKIPLIST_FRAMESIZE 24

typedef struct usf_skipnode {
	struct usf_skipnode *nextnodes[USF_SKIPLIST_FRAMESIZE];
	usf_data data;
	uint64_t index;
} usf_skipnode;

typedef struct usf_skiplist {
	usf_skipnode *base[USF_SKIPLIST_FRAMESIZE];
	uint64_t size;
} usf_skiplist;

usf_skiplist *usf_newsk(void);
usf_skiplist *usf_skset(usf_skiplist *skiplist, uint64_t i, usf_data data);
usf_data usf_skget(usf_skiplist *skiplist, uint64_t data);
usf_data usf_skdel(usf_skiplist *skiplist, uint64_t data);
void usf_freesk(usf_skiplist *skiplist);
void usf_freeskptr(usf_skiplist *skiplist);

#endif
