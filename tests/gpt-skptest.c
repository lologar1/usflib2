#include "usfskiplist.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

/* ============================================================
 * Test utilities
 * ============================================================ */

#define ASSERT(cond, msg) do { \
	if (!(cond)) { \
		fprintf(stderr, "[FAIL] %s (line %d)\n", msg, __LINE__); \
		exit(EXIT_FAILURE); \
	} \
} while (0)

#define TEST(msg) printf("[TEST] %s\n", msg)
#define PASS()    printf("  -> OK\n")

#define STRESS_COUNT  100000
#define RANDOM_OPS    200000
#define THREAD_COUNT  8
#define THREAD_OPS    50000

/* ============================================================
 * Timing helper
 * ============================================================ */

static inline double now_sec(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec * 1e-9;
}

/* ============================================================
 * Individual tests (each uses a fresh skiplist)
 * ============================================================ */

static void test_empty_access(void) {
	TEST("empty skiplist access");

	usf_skiplist *sk = usf_newsk();
	ASSERT(sk != NULL, "usf_newsk failed");

	ASSERT(usf_skget(sk, 0).u == 0, "get on empty should return USFNULL");
	ASSERT(usf_skdel(sk, 0).u == 0, "del on empty should return USFNULL");

	usf_freesk(sk);
	PASS();
}

static void test_sequential_insert_get(void) {
	TEST("sequential insert/get");

	usf_skiplist *sk = usf_newsk();

	for (u64 i = 0; i < 1000; i++)
		usf_skset(sk, i, USFDATAU(i + 1));

	for (u64 i = 0; i < 1000; i++)
		ASSERT(usf_skget(sk, i).u == i + 1, "sequential get failed");

	usf_freesk(sk);
	PASS();
}

static void test_sparse_indices(void) {
	TEST("sparse indices");

	usf_skiplist *sk = usf_newsk();

	usf_skset(sk, 1, USFDATAU(11));
	usf_skset(sk, 1000000, USFDATAU(22));

	ASSERT(usf_skget(sk, 1).u == 11, "sparse index 1 failed");
	ASSERT(usf_skget(sk, 1000000).u == 22, "sparse index large failed");
	ASSERT(usf_skget(sk, 2).u == 0, "unset sparse index should be null");

	usf_freesk(sk);
	PASS();
}

static void test_overwrite(void) {
	TEST("overwrite existing index");

	usf_skiplist *sk = usf_newsk();

	usf_skset(sk, 42, USFDATAU(1));
	usf_skset(sk, 42, USFDATAU(2));

	ASSERT(usf_skget(sk, 42).u == 2, "overwrite did not replace value");

	usf_freesk(sk);
	PASS();
}

static void test_delete(void) {
	TEST("delete semantics");

	usf_skiplist *sk = usf_newsk();

	usf_skset(sk, 7, USFDATAU(77));

	ASSERT(usf_skdel(sk, 7).u == 77, "delete returned wrong value");
	ASSERT(usf_skget(sk, 7).u == 0, "deleted value still accessible");
	ASSERT(usf_skdel(sk, 7).u == 0, "double delete should return null");

	usf_freesk(sk);
	PASS();
}

static void test_mixed_types(void) {
	TEST("mixed usf_data types");

	usf_skiplist *sk = usf_newsk();
	int x = 123;

	usf_skset(sk, 0, USFDATAI(-5));
	usf_skset(sk, 1, USFDATAD(3.14));
	usf_skset(sk, 2, USFDATAP(&x));

	ASSERT(usf_skget(sk, 0).i == -5, "int data mismatch");
	ASSERT(usf_skget(sk, 1).f == 3.14, "double data mismatch");
	ASSERT(usf_skget(sk, 2).p == &x, "pointer data mismatch");

	usf_freesk(sk);
	PASS();
}

static void test_null_skiplist(void) {
	TEST("NULL skiplist safety");

	ASSERT(usf_skget(NULL, 0).u == 0, "get(NULL) should be null");
	ASSERT(usf_skdel(NULL, 0).u == 0, "del(NULL) should be null");

	usf_freesk(NULL);
	usf_freeskptr(NULL);

	PASS();
}

/* ============================================================
 * Stress & randomized tests
 * ============================================================ */

static void test_stress(void) {
	TEST("stress sequential insert/get/delete");

	usf_skiplist *sk = usf_newsk();

	for (u64 i = 0; i < STRESS_COUNT; i++)
		usf_skset(sk, i, USFDATAU(i));

	for (u64 i = 0; i < STRESS_COUNT; i++)
		ASSERT(usf_skget(sk, i).u == i, "stress get failed");

	for (u64 i = 0; i < STRESS_COUNT; i++)
		ASSERT(usf_skdel(sk, i).u == i, "stress delete failed");

	usf_freesk(sk);
	PASS();
}

static void test_randomized(void) {
	TEST("randomized operations");

	usf_skiplist *sk = usf_newsk();

	for (int i = 0; i < RANDOM_OPS; i++) {
		u64 idx = rand() % 10000;
		int op = rand() % 3;

		if (op == 0)
			usf_skset(sk, idx, USFDATAU(idx + 1));
		else if (op == 1)
			usf_skget(sk, idx);
		else
			usf_skdel(sk, idx);
	}

	usf_freesk(sk);
	PASS();
}

/* ============================================================
 * Thread-safe tests
 * ============================================================ */

typedef struct {
	usf_skiplist *sk;
	int id;
} thread_ctx;

static void *thread_worker(void *arg) {
	thread_ctx *ctx = arg;
	u64 base = (u64)ctx->id * THREAD_OPS;

	for (u64 i = 0; i < THREAD_OPS; i++) {
		u64 idx = base + i;
		usf_skset(ctx->sk, idx, USFDATAU(idx));
		ASSERT(usf_skget(ctx->sk, idx).u == idx, "thread get failed");
	}

	return NULL;
}

static void test_threadsafe(void) {
	TEST("thread-safe concurrent access");

	usf_skiplist *sk = usf_newsk_ts();
	ASSERT(sk != NULL, "usf_newsk_ts failed");

	pthread_t threads[THREAD_COUNT];
	thread_ctx ctx[THREAD_COUNT];

	for (int i = 0; i < THREAD_COUNT; i++) {
		ctx[i].sk = sk;
		ctx[i].id = i;
		pthread_create(&threads[i], NULL, thread_worker, &ctx[i]);
	}

	for (int i = 0; i < THREAD_COUNT; i++)
		pthread_join(threads[i], NULL);

	usf_freesk(sk);
	PASS();
}

/* ============================================================
 * Performance benchmarks (fresh skiplists)
 * ============================================================ */

static void benchmark(const char *name, usf_skiplist *sk) {
	printf("[BENCH] %s\n", name);

	double t0 = now_sec();
	for (u64 i = 0; i < STRESS_COUNT; i++)
		usf_skset(sk, i, USFDATAU(i));
	double t1 = now_sec();

	for (u64 i = 0; i < STRESS_COUNT; i++)
		usf_skget(sk, i);
	double t2 = now_sec();

	for (u64 i = 0; i < STRESS_COUNT; i++)
		usf_skdel(sk, i);
	double t3 = now_sec();

	printf("  insert: %.3f ms\n", (t1 - t0) * 1000.0);
	printf("  get   : %.3f ms\n", (t2 - t1) * 1000.0);
	printf("  delete: %.3f ms\n", (t3 - t2) * 1000.0);
}

/* ============================================================
 * Main
 * ============================================================ */

int main(void) {
	srand((unsigned)time(NULL));

	printf("==== Skiplist Test Suite ====\n");

	test_null_skiplist();
	test_empty_access();
	test_sequential_insert_get();
	test_sparse_indices();
	test_overwrite();
	test_delete();
	test_mixed_types();
	test_stress();
	test_randomized();
	test_threadsafe();

	usf_skiplist *sk_fast = usf_newsk();
	usf_skiplist *sk_ts   = usf_newsk_ts();

	benchmark("non-thread-safe", sk_fast);
	benchmark("thread-safe", sk_ts);

	usf_freesk(sk_fast);
	usf_freesk(sk_ts);

	printf("==== ALL TESTS PASSED ====\n");
	return 0;
}

