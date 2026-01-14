#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

/* ===== Library header ===== */
#include "usfhashmap.h"

/* ===== Test helpers ===== */

#define TEST_ASSERT(cond) do {                                      \
    if (!(cond)) {                                                  \
        fprintf(stderr, "ASSERT FAILED: %s (%s:%d)\n",              \
                #cond, __FILE__, __LINE__);                          \
        abort();                                                     \
    }                                                               \
} while (0)

static inline double now_sec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static char *mkkey(u64 i)
{
    char *k = malloc(32);
    TEST_ASSERT(k);
    snprintf(k, 32, "key_%llu", (unsigned long long)i);
    return k;
}

/* ===== Correctness tests ===== */

static void test_string_basic(void)
{
    usf_hashmap *hm = usf_newhm();
    TEST_ASSERT(hm);

    char *a = strdup("alpha");
    char *b = strdup("beta");

    TEST_ASSERT(usf_strhmget(hm, a).u == 0);

    usf_strhmput(hm, a, USFDATAU(1));
    TEST_ASSERT(usf_strhmget(hm, a).u == 1);

    usf_strhmput(hm, a, USFDATAU(2));
    TEST_ASSERT(usf_strhmget(hm, a).u == 2);

    usf_strhmput(hm, b, USFDATAU(3));
    TEST_ASSERT(usf_strhmget(hm, b).u == 3);

    TEST_ASSERT(usf_strhmdel(hm, a).u == 2);
    TEST_ASSERT(usf_strhmget(hm, a).u == 0);

    free(a);
    free(b);
    usf_freestrhm(hm);
}

static void test_int_basic(void)
{
    usf_hashmap *hm = usf_newhmsz(128);
    TEST_ASSERT(hm);

    TEST_ASSERT(usf_inthmget(hm, 42).u == 0);

    usf_inthmput(hm, 42, USFDATAU(99));
    TEST_ASSERT(usf_inthmget(hm, 42).u == 99);

    usf_inthmput(hm, 42, USFDATAU(123));
    TEST_ASSERT(usf_inthmget(hm, 42).u == 123);

    TEST_ASSERT(usf_inthmdel(hm, 42).u == 123);
    TEST_ASSERT(usf_inthmget(hm, 42).u == 0);

    usf_freeinthm(hm);
}

static void test_string_iteration(void)
{
    const u64 N = 1000;
    usf_hashmap *hm = usf_newhm();
    TEST_ASSERT(hm);

    for (u64 i = 0; i < N; i++) {
        char *k = mkkey(i);
        usf_strhmput(hm, k, USFDATAU(i + 1));
        free(k);
    }

    u64 iter = 0, count = 0;
    for (;;) {
        usf_data *v = usf_strhmnext(hm, &iter);
        if (!v)
            break;
        TEST_ASSERT(v->u != 0);
        count++;
    }

    TEST_ASSERT(count == N);
    usf_freestrhm(hm);
}

static void test_resize_int(void)
{
    usf_hashmap *hm = usf_newhmsz(64);
    TEST_ASSERT(hm);

    for (u64 i = 0; i < 1000; i++)
        usf_inthmput(hm, i, USFDATAU(i * 2));

    usf_resizeinthm(hm, 4096);

    for (u64 i = 0; i < 1000; i++)
        TEST_ASSERT(usf_inthmget(hm, i).u == i * 2);

    usf_freeinthm(hm);
}

static void test_string_stress(void)
{
    const u64 N = 5000;
    usf_hashmap *hm = usf_newhm();
    TEST_ASSERT(hm);

    for (u64 i = 0; i < N; i++) {
        char *k = mkkey(i);
        usf_strhmput(hm, k, USFDATAU(i));
        free(k);
    }

    for (u64 i = 0; i < N; i += 2) {
        char *k = mkkey(i);
        TEST_ASSERT(usf_strhmdel(hm, k).u == i);
        free(k);
    }

    for (u64 i = 1; i < N; i += 2) {
        char *k = mkkey(i);
        TEST_ASSERT(usf_strhmget(hm, k).u == i);
        free(k);
    }

    usf_freestrhm(hm);
}

/* ===== Concurrency tests ===== */

typedef struct {
    usf_hashmap *hm;
    u64 start;
    u64 count;
} thread_ctx;

static void *thread_worker(void *arg)
{
    thread_ctx *c = arg;

    for (u64 i = 0; i < c->count; i++) {
        u64 k = c->start + i;
        usf_inthmput(c->hm, k, USFDATAU(k));
    }

    for (u64 i = 0; i < c->count; i++) {
        u64 k = c->start + i;
        TEST_ASSERT(usf_inthmget(c->hm, k).u == k);
    }

    return NULL;
}

static void test_concurrent_int(void)
{
    const int THREADS = 8;
    const u64 PER_THREAD = 10000;

    usf_hashmap *hm = usf_newhmsz_ts(THREADS * PER_THREAD * 2);
    TEST_ASSERT(hm);

    pthread_t t[THREADS];
    thread_ctx ctx[THREADS];

    for (int i = 0; i < THREADS; i++) {
        ctx[i] = (thread_ctx){
            .hm = hm,
            .start = (u64)i * PER_THREAD,
            .count = PER_THREAD
        };
        pthread_create(&t[i], NULL, thread_worker, &ctx[i]);
    }

    for (int i = 0; i < THREADS; i++)
        pthread_join(t[i], NULL);

    for (u64 i = 0; i < THREADS * PER_THREAD; i++)
        TEST_ASSERT(usf_inthmget(hm, i).u == i);

    usf_freeinthm(hm);
}

/* ===== Benchmarks ===== */

static void bench_int(u64 n)
{
    usf_hashmap *hm = usf_newhmsz(n * 2);
    TEST_ASSERT(hm);

    double t0, t1;

    t0 = now_sec();
    for (u64 i = 0; i < n; i++)
        usf_inthmput(hm, i, USFDATAU(i));
    t1 = now_sec();
    double insert = t1 - t0;

    t0 = now_sec();
    for (u64 i = 0; i < n; i++)
        usf_inthmget(hm, i);
    t1 = now_sec();
    double hit = t1 - t0;

    t0 = now_sec();
    for (u64 i = n; i < 2 * n; i++)
        usf_inthmget(hm, i);
    t1 = now_sec();
    double miss = t1 - t0;

    t0 = now_sec();
    for (u64 i = 0; i < n; i++)
        usf_inthmdel(hm, i);
    t1 = now_sec();
    double del = t1 - t0;

    printf("n=%-8llu | ins %.6f | hit %.6f | miss %.6f | del %.6f\n",
           (unsigned long long)n, insert, hit, miss, del);

    usf_freeinthm(hm);
}

/* ===== Main ===== */

int main(void)
{
    printf("=== Correctness tests ===\n");
    test_string_basic();
    test_int_basic();
    test_string_iteration();
    test_resize_int();
    test_string_stress();
    printf("Correctness OK\n\n");

    printf("=== Concurrency tests ===\n");
    test_concurrent_int();
    printf("Concurrency OK\n\n");

    printf("=== Benchmarks ===\n");
    for (u64 n = 1 << 10; n <= (1 << 20); n <<= 2)
        bench_int(n);

    return 0;
}

