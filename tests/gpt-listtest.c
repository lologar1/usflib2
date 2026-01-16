#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "usflist.h"

/* ------------------------------------------------------------
 * Utilities
 * ------------------------------------------------------------ */

#define TEST(name) \
    do { \
        printf("[TEST] %s\n", name); \
    } while (0)

#define ASSERT_ZERO(v) assert((v) == 0)
#define ASSERT_EQ(a, b) assert((a) == (b))

/* ------------------------------------------------------------
 * Generic tests (macro-based per type)
 * ------------------------------------------------------------ */

#define LIST_TESTS(_TYPE, _NAME, _ZERO) \
static void test_##_NAME##_basic(void) { \
    TEST(#_NAME " basic"); \
    usf_list##_NAME *l = usf_newlist##_NAME(); \
    assert(l); \
    assert(l->size == 0); \
    assert(l->capacity == USF_LIST_DEFAULTSIZE); \
    usf_freelist##_NAME(l); \
} \
\
static void test_##_NAME##_set_get(void) { \
    TEST(#_NAME " set/get"); \
    usf_list##_NAME *l = usf_newlist##_NAME##sz(4); \
    usf_list##_NAME##set(l, 0, (_TYPE)1); \
    usf_list##_NAME##set(l, 1, (_TYPE)2); \
    usf_list##_NAME##set(l, 2, (_TYPE)3); \
    ASSERT_EQ(usf_list##_NAME##get(l, 0), (_TYPE)1); \
    ASSERT_EQ(usf_list##_NAME##get(l, 1), (_TYPE)2); \
    ASSERT_EQ(usf_list##_NAME##get(l, 2), (_TYPE)3); \
    usf_freelist##_NAME(l); \
} \
\
static void test_##_NAME##_resize_sparse(void) { \
    TEST(#_NAME " sparse resize"); \
    usf_list##_NAME *l = usf_newlist##_NAME##sz(2); \
    usf_list##_NAME##set(l, 10, (_TYPE)42); \
    assert(l->size == 11); \
    ASSERT_EQ(usf_list##_NAME##get(l, 10), (_TYPE)42); \
    for (u64 i = 0; i < 10; i++) { \
        ASSERT_EQ(usf_list##_NAME##get(l, i), _ZERO); \
    } \
    usf_freelist##_NAME(l); \
} \
\
static void test_##_NAME##_add(void) { \
    TEST(#_NAME " add"); \
    usf_list##_NAME *l = usf_newlist##_NAME##sz(2); \
    usf_list##_NAME##add(l, (_TYPE)5); \
    usf_list##_NAME##add(l, (_TYPE)6); \
    usf_list##_NAME##add(l, (_TYPE)7); \
    assert(l->size == 3); \
    ASSERT_EQ(usf_list##_NAME##get(l, 0), (_TYPE)5); \
    ASSERT_EQ(usf_list##_NAME##get(l, 1), (_TYPE)6); \
    ASSERT_EQ(usf_list##_NAME##get(l, 2), (_TYPE)7); \
    usf_freelist##_NAME(l); \
} \
\
static void test_##_NAME##_insert(void) { \
    TEST(#_NAME " insert"); \
    usf_list##_NAME *l = usf_newlist##_NAME##sz(2); \
    usf_list##_NAME##add(l, (_TYPE)1); \
    usf_list##_NAME##add(l, (_TYPE)3); \
    usf_list##_NAME##ins(l, 1, (_TYPE)2); \
    assert(l->size == 3); \
    ASSERT_EQ(usf_list##_NAME##get(l, 0), (_TYPE)1); \
    ASSERT_EQ(usf_list##_NAME##get(l, 1), (_TYPE)2); \
    ASSERT_EQ(usf_list##_NAME##get(l, 2), (_TYPE)3); \
    usf_freelist##_NAME(l); \
} \
\
static void test_##_NAME##_delete(void) { \
    TEST(#_NAME " delete"); \
    usf_list##_NAME *l = usf_newlist##_NAME##sz(4); \
    usf_list##_NAME##add(l, (_TYPE)1); \
    usf_list##_NAME##add(l, (_TYPE)2); \
    usf_list##_NAME##add(l, (_TYPE)3); \
    _TYPE d = usf_list##_NAME##del(l, 1); \
    ASSERT_EQ(d, (_TYPE)2); \
    assert(l->size == 2); \
    ASSERT_EQ(usf_list##_NAME##get(l, 0), (_TYPE)1); \
    ASSERT_EQ(usf_list##_NAME##get(l, 1), (_TYPE)3); \
    usf_freelist##_NAME(l); \
} \
\
static void test_##_NAME##_oob(void) { \
    TEST(#_NAME " out-of-bounds"); \
    usf_list##_NAME *l = usf_newlist##_NAME(); \
    ASSERT_EQ(usf_list##_NAME##get(l, 100), _ZERO); \
    ASSERT_EQ(usf_list##_NAME##del(l, 0), _ZERO); \
    usf_freelist##_NAME(l); \
} \
\
static void test_##_NAME##_null(void) { \
    TEST(#_NAME " NULL safety"); \
    assert(usf_list##_NAME##set(NULL, 0, (_TYPE)1) == NULL); \
    ASSERT_EQ(usf_list##_NAME##get(NULL, 0), _ZERO); \
    ASSERT_EQ(usf_list##_NAME##del(NULL, 0), _ZERO); \
} \
\
static void run_##_NAME##_tests(void) { \
    test_##_NAME##_basic(); \
    test_##_NAME##_set_get(); \
    test_##_NAME##_resize_sparse(); \
    test_##_NAME##_add(); \
    test_##_NAME##_insert(); \
    test_##_NAME##_delete(); \
    test_##_NAME##_oob(); \
    test_##_NAME##_null(); \
}

/* ------------------------------------------------------------
 * Instantiate for all list types
 * ------------------------------------------------------------ */

LIST_TESTS(i8,   i8,   0)
LIST_TESTS(i16,  i16,  0)
LIST_TESTS(i32,  i32,  0)
LIST_TESTS(i64,  i64,  0)
LIST_TESTS(u8,   u8,   0)
LIST_TESTS(u16,  u16,  0)
LIST_TESTS(u32,  u32,  0)
LIST_TESTS(u64,  u64,  0)
LIST_TESTS(f32,  f32,  0.0f)
LIST_TESTS(f64,  f64,  0.0)
LIST_TESTS(void*, ptr, NULL)

/* ------------------------------------------------------------
 * usf_data special case
 * ------------------------------------------------------------ */

static void test_data(void) {
    TEST("usf_data");
    usf_list *l = usf_newlist();
    usf_data d1 = {0};
    usf_data d2 = {0};
    d1.u64 = 42;
    d2.u64 = 99;
    usf_listset(l, 0, d1);
    usf_listadd(l, d2);
    ASSERT_EQ(usf_listget(l, 0).u64, 42);
    ASSERT_EQ(usf_listget(l, 1).u64, 99);
    usf_freelist(l);
}

/* ------------------------------------------------------------
 * Thread-safe sanity test
 * ------------------------------------------------------------ */

static void test_thread_safe(void) {
    TEST("thread-safe basic");
    usf_listi32 *l = usf_newlisti32_ts();
    assert(l && l->lock);
    usf_listi32add(l, 1);
    usf_listi32add(l, 2);
    ASSERT_EQ(usf_listi32get(l, 1), 2);
    usf_freelisti32(l);
}

/* ------------------------------------------------------------
 * Main
 * ------------------------------------------------------------ */

int main(void) {
    run_i8_tests();
    run_i16_tests();
    run_i32_tests();
    run_i64_tests();
    run_u8_tests();
    run_u16_tests();
    run_u32_tests();
    run_u64_tests();
    run_f32_tests();
    run_f64_tests();
    run_ptr_tests();
    test_data();
    test_thread_safe();

    printf("All tests passed.\n");
    return 0;
}

/* ------------------------------------------------------------
 * usf_data list tests
 * ------------------------------------------------------------ */

static void test_usf_data_basic(void) {
    TEST("usf_data basic");
    usf_list *l = usf_newlist();
    assert(l);
    assert(l->size == 0);
    usf_freelist(l);
}

static void test_usf_data_set_get(void) {
    TEST("usf_data set/get");

    usf_list *l = usf_newlistsz(4);

    usf_listset(l, 0, USFDATAU(42));
    usf_listset(l, 1, USFDATAI(-7));
    usf_listset(l, 2, USFDATAD(3.5));

    assert(usf_listget(l, 0).u == 42);
    assert(usf_listget(l, 1).i == -7);
    assert(usf_listget(l, 2).d == 3.5);

    usf_freelist(l);
}

static void test_usf_data_sparse_resize(void) {
    TEST("usf_data sparse resize");

    usf_list *l = usf_newlistsz(2);
    usf_listset(l, 10, USFDATAU(99));

    assert(l->size == 11);

    for (u64 i = 0; i < 10; i++) {
        usf_data d = usf_listget(l, i);
        assert(d.u == 0);   /* must be zero-initialized */
    }

    assert(usf_listget(l, 10).u == 99);

    usf_freelist(l);
}

static void test_usf_data_add_insert(void) {
    TEST("usf_data add/insert");

    usf_list *l = usf_newlist();

    usf_listadd(l, USFDATAU(1));
    usf_listadd(l, USFDATAU(3));
    usf_listins(l, 1, USFDATAU(2));

    assert(l->size == 3);
    assert(usf_listget(l, 0).u == 1);
    assert(usf_listget(l, 1).u == 2);
    assert(usf_listget(l, 2).u == 3);

    usf_freelist(l);
}

static void test_usf_data_delete(void) {
    TEST("usf_data delete");

    usf_list *l = usf_newlist();

    usf_listadd(l, USFDATAU(10));
    usf_listadd(l, USFDATAU(20));
    usf_listadd(l, USFDATAU(30));

    usf_data d = usf_listdel(l, 1);

    assert(d.u == 20);
    assert(l->size == 2);
    assert(usf_listget(l, 0).u == 10);
    assert(usf_listget(l, 1).u == 30);

    usf_freelist(l);
}

static void test_usf_data_pointer(void) {
    TEST("usf_data pointer");

    int x = 123;
    int y = 456;

    usf_list *l = usf_newlist();

    usf_listadd(l, USFDATAP(&x));
    usf_listadd(l, USFDATAP(&y));

    assert(*(int *)usf_listget(l, 0).p == 123);
    assert(*(int *)usf_listget(l, 1).p == 456);

    usf_freelist(l);
}

static void test_usf_data_oob_and_null(void) {
    TEST("usf_data oob/null");

    usf_list *l = usf_newlist();

    assert(usf_listget(l, 100).u == 0);
    assert(usf_listdel(l, 0).u == 0);

    assert(usf_listget(NULL, 0).u == 0);
    assert(usf_listdel(NULL, 0).u == 0);
    assert(usf_listset(NULL, 0, USFDATAU(1)) == NULL);

    usf_freelist(l);
}

static void run_usf_data_tests(void) {
    test_usf_data_basic();
    test_usf_data_set_get();
    test_usf_data_sparse_resize();
    test_usf_data_add_insert();
    test_usf_data_delete();
    test_usf_data_pointer();
    test_usf_data_oob_and_null();
}

