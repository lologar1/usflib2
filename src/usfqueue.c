#include "usfqueue.h"

usf_queue *usf_newqueue(void) {
	/* Creates a new non thread-safe usf_queue, initialized to 0.
	 * Returns the created queue. */

	usf_queue *queue;
	queue = usf_calloc(1, sizeof(usf_queue));

	return queue;
}

usf_queue *usf_newqueue_ts(void) {
	/* Creates a new thread-safe usf_queue, initialized to 0.
	 * Returns the created queue, or NULL if a mutex cannot be created. */

	usf_queue *queue;
	queue = usf_malloc(sizeof(usf_queue));
	queue->lock = usf_malloc(sizeof(pthread_mutex_t));
	if (pthread_mutex_init(queue->lock, NULL)) { /* Default attributes */
		usf_free(queue);
		return NULL; /* mutex init failed */
	}
	queue->first = queue->last = NULL;

	return queue;
}

usf_queue *usf_enqueue(usf_queue *queue, usf_data data) {
	/* This function is thread-safe when operating on thread-safe queues.
	 *
	 * Enqueues the given data to this FIFO queue.
	 * Returns the queue, or NULL on error. */

	if (queue == NULL) return NULL;
	if (queue->lock) pthread_mutex_lock(queue->lock); /* Thread-safe lock */

	usf_queuenode *enqueue;
	enqueue = usf_malloc(sizeof(usf_queuenode));
	enqueue->data = data;
	enqueue->next = NULL; /* Last in line */

	if (queue->last == NULL) {
		queue->first = queue->last = enqueue; /* First insertion */
	} else {
		queue->last->next = enqueue; /* Append */
		queue->last = enqueue;
	}

	if (queue->lock) pthread_mutex_unlock(queue->lock); /* Thread-safe unlock */
	return queue;
}

usf_data usf_dequeue(usf_queue *queue) {
	/* This function is thread-safe when operating on thread-safe queues.
	 *
	 * Returns dequeued data from this FIFO queue,
	 * or USFNULL (zero) if it is inaccessible. */

	if (queue == NULL) return USFNULL;
	if (queue->lock) pthread_mutex_lock(queue->lock); /* Thread-safe lock */

	usf_queuenode *dequeue;
	if ((dequeue = queue->first) == NULL) {
		if (queue->lock) pthread_mutex_unlock(queue->lock); /* Thread-safe unlock */
		return USFNULL; /* Empty queue */
	}

	usf_data data;
	data = dequeue->data;

	if ((queue->first = dequeue->next) == NULL) /* Bring next one in */
		queue->last = NULL; /* Dequeue was last member */
	usf_free(dequeue);

	if (queue->lock) pthread_mutex_unlock(queue->lock); /* Thread-safe unlock */
	return data;
}

void usf_freequeuefunc(usf_queue *queue, void (*freefunc)(void *)) {
	/* Frees a usf_queue and calls freefunc on its values.
	 * If freefunc is NULL, nothing is done on the queue values.
	 * If queue is NULL, this function has no effect. */

	if (queue == NULL) return;

	usf_queuenode *node, *next;
	for (node = queue->first; node; node = next) {
		next = node->next;
		if (freefunc) freefunc(node->data.p);
		usf_free(node);
	}

	if (queue->lock) {
		pthread_mutex_destroy(queue->lock);
		usf_free(queue->lock);
	}
	usf_free(queue);
}

void usf_freequeueptr(usf_queue *queue) {
	/* Frees a usf_queue and calls usf_free on its values.
	 * If queue is NULL, this function has no effect. */

	usf_freequeuefunc(queue, usf_free);
}

void usf_freequeue(usf_queue *queue) {
	/* Frees a usf_queue without calling usf_free on its values.
	 * If queue is NULL, this function has no effect. */

	usf_freequeuefunc(queue, NULL);
}
