#include "usfqueue.h"

usf_queue *usf_newqueue(void) {
	/* Make new queue */
	usf_queue *queue;

	queue = malloc(sizeof(usf_queue));
	queue->first = queue->last = NULL;

	return queue;
}

usf_queue *usf_enqueue(usf_queue *queue, usf_data d) {
	/* Enqueue to FIFO queue */
	usf_queuenode *append;

	if (queue == NULL) return NULL;

	append = malloc(sizeof(usf_queuenode));
	append->data = d;
	append->next = NULL; /* Last in line */

	if (queue->last == NULL) queue->first = queue->last = append; /* Was empty */
	else {
		queue->last->next = append;
		queue->last = append;
	}

	return queue;
}

usf_data usf_dequeue(usf_queue *queue) {
	/* Dequeue from FIFO queue */
	usf_queuenode *node;
	usf_data data;

	node = queue->first;
	if (queue == NULL || node == NULL) return USFNULL; /* Queue is NULL or is empty */

	data = node->data; /* Retrieve data */

	if ((queue->first = node->next) == NULL) queue->last = NULL; /* Adjust queue */
	free(node);

	return data;
}

void usf_freequeue(usf_queue *queue) {
	/* Frees all nodes and the queue itself */
	usf_queuenode *node, *next;

	for (node = queue->first; node; node = next) {
		next = node->next;
		free(node);
	}

	free(queue);
}

void usf_freequeueptr(usf_queue *queue) {
	/* Frees all nodes and the queue itself, and its data */
	usf_queuenode *node, *next;

	for (node = queue->first; node; node = next) {
		next = node->next;
		free(node->data.p);
		free(node);
	}

	free(queue);
}
