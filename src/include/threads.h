/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#include <time.h>

#define __STDC_VERSION_THREADS_H__ 202311L

#define thread_local _Thread_local
#define ONCE_FLAG_INIT 0
#define TSS_DTOR_ITERATIONS 4

/* same sizes as glibc */
typedef unsigned long thrd_t;
typedef union { char __size[40]; long __align; } mtx_t;
typedef union { char __size[48]; long long __align; } cnd_t;
typedef int once_flag;
typedef unsigned int tss_t;

typedef int (*thrd_start_t)(void*);
typedef void (*tss_dtor_t)(void*);

enum
{
    mtx_plain = 0,
    mtx_recursive = 1,
    mtx_timed = 2
};

enum
{
    thrd_success = 0,
    thrd_nomem,
    thrd_timedout,
    thrd_busy,
    thrd_error
};

/* initialization functions */
void call_once(once_flag* flag, void (*func)(void));

/* condition variable functions */
int cnd_broadcast(cnd_t* cond);
void cnd_destroy(cnd_t* cond);
int cnd_init(cnd_t* cond);
int cnd_signal(cnd_t* cond);
int cnd_timedwait(cnd_t* restrict cond, mtx_t* restrict mtx, const struct timespec* restrict ts);
int cnd_wait(cnd_t* cond, mtx_t* mtx);

/* mutex functions */
void mtx_destroy(mtx_t* mtx);
int mtx_init(mtx_t* mtx, int type);
int mtx_lock(mtx_t* mtx);
int mtx_timedlock(mtx_t* restrict mtx, const struct timespec* restrict ts);
int mtx_trylock(mtx_t* mtx);
int mtx_unlock(mtx_t* mtx);

/* thread functions */
int thrd_create(thrd_t* thr, thrd_start_t func, void* _Opt arg);
thrd_t thrd_current(void);
int thrd_detach(thrd_t thr);
int thrd_equal(thrd_t thr0, thrd_t thr1);
[[noreturn]] void thrd_exit(int res);
int thrd_join(thrd_t thr, int* _Opt res);
int thrd_sleep(const struct timespec* duration, struct timespec* _Opt remaining);
void thrd_yield(void);

/* thread-specific storage functions */
int tss_create(tss_t* key, tss_dtor_t _Opt dtor);
void tss_delete(tss_t key);
void* _Opt tss_get(tss_t key);
int tss_set(tss_t key, void* _Opt val);

#else
#include_next <threads.h>
#endif
