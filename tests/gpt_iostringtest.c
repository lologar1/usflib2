#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "usfio.h"
#include "usfstring.h"

/* ===================== TIMING ===================== */

static double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

#define TEST(name) \
    do { printf("\n[TEST] %s\n", name); } while (0)

#define TIMING(label, expr) \
    do { \
        double t0 = now_sec(); \
        expr; \
        double t1 = now_sec(); \
        printf("  %-30s : %.6f s\n", label, t1 - t0); \
    } while (0)

/* ===================== FILE HELPERS ===================== */

static void write_text_file(const char *path, const char *text) {
    FILE *f = fopen(path, "w");
    assert(f);
    fputs(text, f);
    fclose(f);
}

/* ===================== FILE → STRING ===================== */

static void test_ftos(void) {
    TEST("usf_ftos");

    write_text_file("t.txt", "hello\nworld\n");

    u64 len = 0;
    char *s = usf_ftos("t.txt", &len);

    assert(s);
    assert(len == strlen("hello\nworld\n"));
    assert(strcmp(s, "hello\nworld\n") == 0);

    free(s);
}

/* ===================== FILE → TEXT ===================== */

static void test_ftot_ftost(void) {
    TEST("usf_ftot / usf_ftost");

    write_text_file("t.txt", "a\nb\nc\n");

    u64 l1 = 0, l2 = 0;
    char **t1 = usf_ftot("t.txt", &l1);
    char **t2 = usf_ftost("t.txt", &l2);

    assert(t1 && t2);
    assert(l1 == 3 && l2 == 3);

    for (u64 i = 0; i < 3; i++)
        assert(strcmp(t1[i], t2[i]) == 0);

    /* Ownership semantics */
    usf_freetxt(t1, l1);
    usf_freetxt(t2, 1);
}

/* ===================== BINARY IO ===================== */

static void test_binary(void) {
    TEST("binary I/O");

    const u64 size = 1024 * 1024;
    unsigned char *buf = malloc(size);
    assert(buf);

    for (u64 i = 0; i < size; i++)
        buf[i] = (unsigned char)(i & 0xFF);

    assert(usf_btof("bin.dat", buf, size) == size);

    u64 read_size = 0;
    unsigned char *in = usf_ftob("bin.dat", &read_size);

    assert(in);
    assert(read_size == size);
    assert(memcmp(buf, in, size) == 0);

    free(buf);
    free(in);
}

/* ===================== STRING ===================== */

static void test_string_ops(void) {
    TEST("string ops");

    char s[] = "foobar";

    assert(usf_sstartswith(s, "foo") == s + 3);
    assert(usf_sstartswith(s, "") == s);
    assert(usf_sstartswith(s, "x") == NULL);

    assert(usf_sendswith(s, "bar") == s + 3);
    assert(usf_sendswith(s, "") == s + strlen(s));
    assert(usf_sendswith(s, "x") == NULL);

    assert(usf_scount("aaab", 'a') == 3);
    assert(usf_scount("", 'x') == 0);

    char r[] = "a-b-c-d";
    assert(usf_sreplace(r, '-', '_') == 3);
    assert(strcmp(r, "a_b_c_d") == 0);
}

/* ===================== TEXT ARRAYS ===================== */

static void test_text_array_ops(void) {
    TEST("text array ops");

    char *t[] = { "one", "two", "three" };

    assert(usf_txtcontainsline(t, 3, "two"));
    assert(!usf_txtcontainsline(t, 3, "four"));

    usf_reversetxtlines(t, 3);
    assert(strcmp(t[0], "three") == 0);
    assert(strcmp(t[2], "one") == 0);
}

/* ===================== SPLIT ===================== */

static void test_scsplit(void) {
    TEST("usf_scsplit");

    char s[] = "a,b,,c";
    u64 n = 0;

    char **v = usf_scsplit(s, ',', &n);
    assert(v && n == 4);

    assert(strcmp(v[0], "a") == 0);
    assert(strcmp(v[1], "b") == 0);
    assert(strcmp(v[2], "") == 0);
    assert(strcmp(v[3], "c") == 0);

    free(v);
}

/* ===================== STRCAT ===================== */

static void test_strcat(void) {
    TEST("usf_strcat");

    char buf[32] = {0};
    assert(usf_strcat(buf, sizeof(buf), 3, "ab", "cd", "ef") == 0);
    assert(strcmp(buf, "abcdef") == 0);

    char small[5] = {0};
    assert(usf_strcat(small, sizeof(small), 2, "abc", "de") == -1);
}

/* ===================== PERFORMANCE ===================== */

static void perf_tests(void) {
    TEST("performance");

    /* Large text file */
    const u64 lines = 200000;
    FILE *f = fopen("big.txt", "w");
    assert(f);
    for (u64 i = 0; i < lines; i++)
        fputs("abcdefghijklmnopqrstuvwxyz0123456789\n", f);
    fclose(f);

    u64 l;
    char **t;

    TIMING("ftost (200k lines)", {
        t = usf_ftost("big.txt", &l);
        assert(l == lines);
        usf_freetxt(t, 1);
    });

    TIMING("ftot (200k lines)", {
        t = usf_ftot("big.txt", &l);
        assert(l == lines);
        usf_freetxt(t, l);
    });

    /* String ops */
    char *big = malloc(10 * 1024 * 1024);
    assert(big);
    memset(big, 'a', 10 * 1024 * 1024 - 1);
    big[10 * 1024 * 1024 - 1] = 0;

    TIMING("scount (10MB)", {
        volatile u64 c = usf_scount(big, 'a');
        (void)c;
    });

    TIMING("sreplace (10MB)", {
        volatile u64 c = usf_sreplace(big, 'a', 'b');
        (void)c;
    });

    free(big);
}

/* ===================== MAIN ===================== */

int main(void) {
    test_ftos();
    test_ftot_ftost();
    test_binary();
    test_string_ops();
    test_text_array_ops();
    test_scsplit();
    test_strcat();
    perf_tests();

    puts("\nAll tests passed.");
    return 0;
}

