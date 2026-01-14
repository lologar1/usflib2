#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>

#include "usfqueue.h"
#include "usfdata.h"

/* ------------------------------------------------------------
 * Configuration
 * ------------------------------------------------------------ */

#define TEST_ASSERT(cond) \
	do { \
		if (!(cond)) { \
			fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
			abort(); \
		} \
	} while (0)

#define PERF_ITERATIONS 1000000
#define STRESS_ITERATIONS 500000
#define CONCURRENCY_THREADS 8
#define CONCURRENCY_OPS 100000

/* ------------------------------------------------------------
 * Timing Utilities
 * ------------------------------------------------------------ */

static inline double now_sec(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec * 1e-9;
}

/* ------------------------------------------------------------
 * Basic Correctness Tests
 * ------------------------------------------------------------ */

static void test_create_and_free(void) {
	usf_queue *q = usf_newqueue();
	TEST_ASSERT(q != NULL);
	usf_freequeue(q);

	q = usf_newqueue_ts();
	TEST_ASSERT(q != NULL);
	usf_freequeue(q);
}

static void test_empty_dequeue(void) {
	usf_queue *q = usf_newqueue();
	usf_data d = usf_dequeue(q);
	TEST_ASSERT(d.u == 0);
	usf_freequeue(q);
}

static void test_fifo_order(void) {
	usf_queue *q = usf_newqueue();

	for (int i = 0; i < 1000; i++)
		usf_enqueue(q, USFDATAI(i));

	for (int i = 0; i < 1000; i++) {
		usf_data d = usf_dequeue(q);
		TEST_ASSERT(d.i == i);
	}

	usf_freequeue(q);
}

static void test_null_queue_safety(void) {
	TEST_ASSERT(usf_enqueue(NULL, USFDATAI(1)) == NULL);
	usf_data d = usf_dequeue(NULL);
	TEST_ASSERT(d.u == 0);
	usf_freequeue(NULL);
}

/* ------------------------------------------------------------
 * Mixed Data Types
 * ------------------------------------------------------------ */

static void test_mixed_data(void) {
	usf_queue *q = usf_newqueue();

	int x = 42;
	usf_enqueue(q, USFDATAI(-7));
	usf_enqueue(q, USFDATAU(123456));
	usf_enqueue(q, USFDATAD(3.14159));
	usf_enqueue(q, USFDATAP(&x));

	TEST_ASSERT(usf_dequeue(q).i == -7);
	TEST_ASSERT(usf_dequeue(q).u == 123456);
	TEST_ASSERT(usf_dequeue(q).d > 3.1415);
	TEST_ASSERT(*(int *)usf_dequeue(q).p == 42);

	usf_freequeue(q);
}

/* ------------------------------------------------------------
 * Stress Testing
 * ------------------------------------------------------------ */

static void test_stress_fifo(void) {
	usf_queue *q = usf_newqueue();

	for (int i = 0; i < STRESS_ITERATIONS; i++)
		usf_enqueue(q, USFDATAI(i));

	for (int i = 0; i < STRESS_ITERATIONS; i++) {
		usf_data d = usf_dequeue(q);
		TEST_ASSERT(d.i == i);
	}

	usf_freequeue(q);
}

/* ------------------------------------------------------------
 * Performance Benchmark
 * ------------------------------------------------------------ */

static void benchmark_queue(const char *name, usf_queue *q) {
	double start = now_sec();

	for (int i = 0; i < PERF_ITERATIONS; i++)
		usf_enqueue(q, USFDATAI(i));

	for (int i = 0; i < PERF_ITERATIONS; i++)
		usf_dequeue(q);

	double elapsed = now_sec() - start;
	printf("[PERF] %s: %.2f ops/sec\n",
	       name,
	       (PERF_ITERATIONS * 2) / elapsed);
}

static void test_performance(void) {
	usf_queue *q1 = usf_newqueue();
	usf_queue *q2 = usf_newqueue_ts();

	benchmark_queue("non-thread-safe", q1);
	benchmark_queue("thread-safe", q2);

	usf_freequeue(q1);
	usf_freequeue(q2);
}

/* ------------------------------------------------------------
 * Concurrency Testing (Thread-Safe Queue Only)
 * ------------------------------------------------------------ */

typedef struct {
	usf_queue *queue;
	int thread_id;
} worker_ctx;

static void *producer(void *arg) {
	worker_ctx *ctx = arg;
	for (int i = 0; i < CONCURRENCY_OPS; i++) {
		usf_enqueue(ctx->queue,
		            USFDATAI(ctx->thread_id * CONCURRENCY_OPS + i));
	}
	return NULL;
}

static void *consumer(void *arg) {
	worker_ctx *ctx = arg;
	int count = 0;
	while (count < CONCURRENCY_OPS) {
		usf_data d = usf_dequeue(ctx->queue);
		if (d.u != 0)
			count++;
	}
	return NULL;
}

static void test_concurrency(void) {
	pthread_t producers[CONCURRENCY_THREADS];
	pthread_t consumers[CONCURRENCY_THREADS];
	worker_ctx ctx;

	ctx.queue = usf_newqueue_ts();
	TEST_ASSERT(ctx.queue != NULL);

	for (int i = 0; i < CONCURRENCY_THREADS; i++) {
		ctx.thread_id = i;
		pthread_create(&producers[i], NULL, producer, &ctx);
		pthread_create(&consumers[i], NULL, consumer, &ctx);
	}

	for (int i = 0; i < CONCURRENCY_THREADS; i++) {
		pthread_join(producers[i], NULL);
		pthread_join(consumers[i], NULL);
	}

	usf_freequeue(ctx.queue);
}

/* ------------------------------------------------------------
 * Free With Data Destruction
 * ------------------------------------------------------------ */

static void test_freequeueptr(void) {
	usf_queue *q = usf_newqueue();

	for (int i = 0; i < 100; i++) {
		int *p = malloc(sizeof(int));
		*p = i;
		usf_enqueue(q, USFDATAP(p));
	}

	usf_freequeueptr(q); /* should free all p */
}

/* ------------------------------------------------------------
 * Main Test Runner
 * ------------------------------------------------------------ */

int main(void) {
	printf("Running usf_queue test suite...\n");

	test_create_and_free();
	test_empty_dequeue();
	test_fifo_order();
	test_null_queue_safety();
	test_mixed_data();
	test_stress_fifo();
	test_performance();
	test_concurrency();
	test_freequeueptr();

	printf("All tests passed.\n");
	return 0;
}

