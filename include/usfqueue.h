#ifndef USFQUEUE_H
#define USFQUEUE_H

#include "usfdata.h"
#include <stddef.h>
#include <stdlib.h>

typedef struct usf_queuenode {
	usf_data data;
	struct usf_queuenode *next;
} usf_queuenode;

typedef struct usf_queue {
	usf_queuenode *first;
	usf_queuenode *last;
} usf_queue;

usf_queue *usf_newqueue(void);
usf_queue *usf_enqueue(usf_queue *queue, usf_data d);
usf_data usf_dequeue(usf_queue *queue);
void usf_freequeue(usf_queue *queue);
void usf_freequeueptr(usf_queue *queue);

#endif
