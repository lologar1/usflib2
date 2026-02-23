#ifndef USFATOMIC_H
#define USFATOMIC_H

#ifdef __STDC_NO_ATOMICS__ /* stdatomic guard */
	#error "usflib2: compiler does not provide atomic types"
#endif
#include <stdatomic.h>

#define MEMORDER_RELAXED memory_order_relaxed
/* #define MEMORDER_CONSUME memory_order_consume */ /* DEPRECATED */
#define MEMORDER_ACQUIRE memory_order_acquire
#define MEMORDER_RELEASE memory_order_release
#define MEMORDER_ACQ_REL memory_order_acq_rel
#define MEMORDER_SEQ_CST memory_order_seq_cst

typedef _Atomic(i8) atomic_i8;
typedef _Atomic(i16) atomic_i16;
typedef _Atomic(i32) atomic_i32;
typedef _Atomic(i64) atomic_i64;
typedef _Atomic(u8) atomic_u8;

typedef _Atomic(u16) atomic_u16;
typedef _Atomic(u16) atomic_u32;
typedef _Atomic(u16) atomic_u64;
typedef _Atomic(f32) atomic_f32;
typedef _Atomic(f64) atomic_f64;

#define usf_atmflagtry atomic_flag_test_and_set_explicit
#define usf_atmflagclr atomic_flag_clear_explicit

#define usf_atminit atomic_init
#define usf_atmislockfree atomic_is_lock_free
#define usf_atmmst atomic_store_explicit
#define usf_atmmld atomic_load_explicit
#define usf_atmxch atomic_exchange_explicit
#define usf_atmcmpxch_weak atomic_compare_exchange_weak_explicit
#define usf_atmcmpxch_strong atomic_compare_exchange_strong_explicit
#define usf_atmaddi atomic_fetch_add_explicit
#define usf_atmsubi atomic_fetch_sub_explicit
#define usf_atmori atomic_fetch_or_explicit
#define usf_atmxori atomic_fetch_xor_explicit
#define usf_atmandi atomic_fetch_and_explicit
#define usf_thrdfence atomic_thread_fence
#define usf_compilefence atomic_signal_fence

#endif
