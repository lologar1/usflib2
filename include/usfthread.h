#ifndef USFTHREAD_H
#define USFTHREAD_H

#include <threads.h>
#include "usfstd.h"

#ifdef __STDC_NO_THREADS__
	#error "usflib2: compiler does not provide C11 <threads.h>"
#endif

typedef thrd_t usf_thread;
typedef mtx_t usf_mutex;
typedef cnd_t usf_cond;
typedef usf_compatibility_int (*usf_threadfunc)(void *);

#define THRD_SUCCESS thrd_success
#define THRD_NOMEM thrd_nomem
#define THRD_TIMEOUT thrd_timeout
#define THRD_BUSY thrd_busy
#define THRD_ERROR thrd_error
#define MTXINIT_PLAIN mtx_plain
#define MTXINIT_TIMED mtx_timed
#define MTXINIT_RECURSIVE (mtx_plain | mtx_recursive)
#define MTXINIT_TIMEDRECURSIVE (mtx_timed | mtx_recursive)

#define usf_thrdcreate thrd_create
#define usf_thrdequal thrd_equal
#define usf_thrdcurrent thrd_current
#define usf_thrdsleep thrd_sleep
#define usf_thrdyield thrd_yield
#define usf_thrdexit thrd_exit
#define usf_thrddetach thrd_detach
#define usf_thrdjoin thrd_join

#define usf_mtxinit mtx_init
#define usf_mtxlock mtx_lock
#define usf_mtxtimedlock mtx_timedlock
#define usf_mtxtrylock mtx_trylock
#define usf_mtxunlock mtx_unlock
#define usf_mtxdestroy mtx_destroy

#define usf_cndinit cnd_init
#define usf_cndsignal cnd_signal
#define usf_cndbroadcast cnd_broadcase
#define usf_cndwait cnd_wait
#define usf_cndtimedwait cnd_timedwait
#define usf_cnddestroy cnd_destroy

#endif
