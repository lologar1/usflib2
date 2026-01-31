#ifndef USFQUEUE_H
#define USFQUEUE_H

#include <pthread.h>
#include "usfstd.h"
#include "usfdata.h"

typedef struct usf_queuenode {
	usf_data data;
	struct usf_queuenode *next;
} usf_queuenode;

typedef struct usf_queue {
	pthread_mutex_t *lock;
	usf_queuenode *first;
	usf_queuenode *last;
} usf_queue;

usf_queue *usf_newqueue(void);
usf_queue *usf_newqueue_ts(void);

usf_queue *usf_enqueue(usf_queue *queue, usf_data data);	/* Thread-safe */
usf_data usf_dequeue(usf_queue *queue);						/* Thread-safe */

void usf_freequeuefunc(usf_queue *queue, void (*freefunc)(void *));
void usf_freequeueptr(usf_queue *queue);
void usf_freequeue(usf_queue *queue);

#endif
