#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include "usfstring.h"
#include "usfio.h"

/* ===================== UTILITIES ===================== */

#define TEST(name) do { \
    printf("[TEST] %s\n", name); \
} while (0)

static void write_text_file(const char *path, const char *text) {
    FILE *f = fopen(path, "w");
    assert(f);
    fputs(text, f);
    fclose(f);
}

static void write_binary_file(const char *path, const void *data, size_t size) {
    FILE *f = fopen(path, "wb");
    assert(f);
    fwrite(data, 1, size, f);
    fclose(f);
}

static void *xmalloc(size_t n) {
    void *p = malloc(n);
    assert(p);
    return p;
}

/* ===================== FILE → STRING ===================== */

static void test_ftos_basic(void) {
    TEST("usf_ftos basic");

    write_text_file("t.txt", "hello\nworld\n");
    u64 len = 0;
    char *s = usf_ftos("t.txt", &len);

    assert(s);
    assert(len == strlen("hello\nworld\n"));
    assert(strcmp(s, "hello\nworld\n") == 0);

    free(s);
}

static void test_ftos_empty(void) {
    TEST("usf_ftos empty file");

    write_text_file("t.txt", "");
    u64 len = 123;
    char *s = usf_ftos("t.txt", &len);

    assert(s);
    assert(len == 0);
    assert(strcmp(s, "") == 0);

    free(s);
}

/* ===================== FILE → TEXT LINES ===================== */

static void test_ftot_vs_ftost(void) {
    TEST("usf_ftot vs usf_ftost");

    write_text_file("t.txt", "a\nb\nc\n");
    u64 l1 = 0, l2 = 0;

    char **t1 = usf_ftot("t.txt", &l1);
    char **t2 = usf_ftost("t.txt", &l2);

    assert(t1 && t2);
    assert(l1 == 3 && l2 == 3);

    for (u64 i = 0; i < 3; i++) {
        assert(strcmp(t1[i], t2[i]) == 0);
    }

    /* ftot: lines allocated separately */
    assert(t1[0] != t1[1]);

    /* ftost: lines share backing storage */
    assert(t2[0] + strlen(t2[0]) + 1 == t2[1]);

    usf_freetxt(t1, l1);
    free(t2); /* ftost should require only one free */
}

static void test_single_line_file(void) {
    TEST("single line file");

    write_text_file("t.txt", "onlyline\n");
    u64 l = 0;
    char **t = usf_ftot("t.txt", &l);

    assert(t);
    assert(l == 1);
    assert(strcmp(t[0], "onlyline") == 0);

    usf_freetxt(t, l);
}

/* ===================== BINARY IO ===================== */

static void test_binary_roundtrip(void) {
    TEST("binary roundtrip");

    size_t size = 1024 * 1024;
    unsigned char *buf = xmalloc(size);

    for (size_t i = 0; i < size; i++)
        buf[i] = (unsigned char)(i & 0xFF);

    u64 written = usf_btof("bin.dat", buf, size);
    assert(written == size);

    u64 read_size = 0;
    unsigned char *in = usf_ftob("bin.dat", &read_size);

    assert(in);
    assert(read_size == size);
    assert(memcmp(buf, in, size) == 0);

    free(buf);
    free(in);
}

/* ===================== STRING UTILITIES ===================== */

static void test_sstartswith(void) {
    TEST("usf_sstartswith");

    char s[] = "foobar";
    assert(usf_sstartswith(s, "foo") == s + 3);
    assert(usf_sstartswith(s, "") == s);
    assert(usf_sstartswith(s, "foobar") == s + 6);
    assert(usf_sstartswith(s, "foobarbaz") == NULL);
}

static void test_sendswith(void) {
    TEST("usf_sendswith");

    char s[] = "foobar";
    assert(usf_sendswith(s, "bar") == s + 3);
    assert(usf_sendswith(s, "") == s + strlen(s));
    assert(usf_sendswith(s, "foobar") == s);
    assert(usf_sendswith(s, "baz") == NULL);
}

static void test_scount(void) {
    TEST("usf_scount");

    assert(usf_scount("aaaa", 'a') == 4);
    assert(usf_scount("", 'a') == 0);
    assert(usf_scount("abc", 'x') == 0);
}

static void test_sreplace(void) {
    TEST("usf_sreplace");

    char s[] = "a-b-c-d";
    u64 n = usf_sreplace(s, '-', '_');

    assert(n == 3);
    assert(strcmp(s, "a_b_c_d") == 0);
}

/* ===================== TEXT ARRAY OPS ===================== */

static void test_txtcontains_and_reverse(void) {
    TEST("usf_txtcontainsline + reverse");

    char *t[] = { "one", "two", "three" };

    assert(usf_txtcontainsline(t, 3, "two") == 1);
    assert(usf_txtcontainsline(t, 3, "four") == 0);

    usf_reversetxtlines(t, 3);

    assert(strcmp(t[0], "three") == 0);
    assert(strcmp(t[2], "one") == 0);
}

/* ===================== STRING SPLIT ===================== */

static void test_scsplit(void) {
    TEST("usf_scsplit");

    char s[] = "a,b,,c";
    u64 n = 0;
    char **v = usf_scsplit(s, ',', &n);

    assert(v);
    assert(n == 4);

    assert(strcmp(v[0], "a") == 0);
    assert(strcmp(v[1], "b") == 0);
    assert(strcmp(v[2], "") == 0);
    assert(strcmp(v[3], "c") == 0);

    free(v);
}

/* ===================== STRING CONCAT ===================== */

static void test_strcat(void) {
    TEST("usf_strcat");

    char buf[16] = {0};

    assert(usf_strcat(buf, sizeof(buf), 3, "ab", "cd", "ef") == 0);
    assert(strcmp(buf, "abcdef") == 0);

    char small[5] = {0};
    assert(usf_strcat(small, sizeof(small), 2, "abc", "de") == -1);
}

/* ===================== PERFORMANCE SMOKE ===================== */

static void test_large_text(void) {
    TEST("large text performance");

    const size_t lines = 200000;
    FILE *f = fopen("big.txt", "w");
    assert(f);

    for (size_t i = 0; i < lines; i++)
        fputs("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n", f);

    fclose(f);

    u64 l = 0;
    char **t = usf_ftost("big.txt", &l);

    assert(t);
    assert(l == lines);

    free(t);
}

/* ===================== MAIN ===================== */

int main(void) {
    test_ftos_basic();
    test_ftos_empty();

    test_ftot_vs_ftost();
    test_single_line_file();

    test_binary_roundtrip();

    test_sstartswith();
    test_sendswith();
    test_scount();
    test_sreplace();

    test_txtcontains_and_reverse();
    test_scsplit();
    test_strcat();

    test_large_text();

    puts("\nAll tests passed.");
    return 0;
}

