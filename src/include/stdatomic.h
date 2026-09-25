/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

/* gcc's <stdatomic.h> needs statement expressions and __auto_type, and msvc's needs its own intrinsics,
   so only clang uses the system header */
#if defined(CAKE_HEADERS) || !defined(__clang__)

#pragma once

#include <stddef.h>
#include <stdint.h>

#define __STDC_VERSION_STDATOMIC_H__ 202311L

#ifndef __ATOMIC_RELAXED
#define __ATOMIC_RELAXED 0
#define __ATOMIC_CONSUME 1
#define __ATOMIC_ACQUIRE 2
#define __ATOMIC_RELEASE 3
#define __ATOMIC_ACQ_REL 4
#define __ATOMIC_SEQ_CST 5
#endif

#define ATOMIC_BOOL_LOCK_FREE     2
#define ATOMIC_CHAR_LOCK_FREE     2
#define ATOMIC_CHAR8_T_LOCK_FREE  2
#define ATOMIC_CHAR16_T_LOCK_FREE 2
#define ATOMIC_CHAR32_T_LOCK_FREE 2
#define ATOMIC_WCHAR_T_LOCK_FREE  2
#define ATOMIC_SHORT_LOCK_FREE    2
#define ATOMIC_INT_LOCK_FREE      2
#define ATOMIC_LONG_LOCK_FREE     2
#define ATOMIC_LLONG_LOCK_FREE    2
#define ATOMIC_POINTER_LOCK_FREE  2

typedef enum memory_order
{
    memory_order_relaxed = __ATOMIC_RELAXED,
    memory_order_consume = __ATOMIC_CONSUME,
    memory_order_acquire = __ATOMIC_ACQUIRE,
    memory_order_release = __ATOMIC_RELEASE,
    memory_order_acq_rel = __ATOMIC_ACQ_REL,
    memory_order_seq_cst = __ATOMIC_SEQ_CST
} memory_order;

#define kill_dependency(y) (y)

typedef _Atomic(bool)               atomic_bool;
typedef _Atomic(char)               atomic_char;
typedef _Atomic(signed char)        atomic_schar;
typedef _Atomic(unsigned char)      atomic_uchar;
typedef _Atomic(short)              atomic_short;
typedef _Atomic(unsigned short)     atomic_ushort;
typedef _Atomic(int)                atomic_int;
typedef _Atomic(unsigned int)       atomic_uint;
typedef _Atomic(long)               atomic_long;
typedef _Atomic(unsigned long)      atomic_ulong;
typedef _Atomic(long long)          atomic_llong;
typedef _Atomic(unsigned long long) atomic_ullong;
typedef _Atomic(unsigned char)      atomic_char8_t;
typedef _Atomic(uint_least16_t)     atomic_char16_t;
typedef _Atomic(uint_least32_t)     atomic_char32_t;
typedef _Atomic(wchar_t)            atomic_wchar_t;
typedef _Atomic(int_least8_t)       atomic_int_least8_t;
typedef _Atomic(uint_least8_t)      atomic_uint_least8_t;
typedef _Atomic(int_least16_t)      atomic_int_least16_t;
typedef _Atomic(uint_least16_t)     atomic_uint_least16_t;
typedef _Atomic(int_least32_t)      atomic_int_least32_t;
typedef _Atomic(uint_least32_t)     atomic_uint_least32_t;
typedef _Atomic(int_least64_t)      atomic_int_least64_t;
typedef _Atomic(uint_least64_t)     atomic_uint_least64_t;
typedef _Atomic(int_fast8_t)        atomic_int_fast8_t;
typedef _Atomic(uint_fast8_t)       atomic_uint_fast8_t;
typedef _Atomic(int_fast16_t)       atomic_int_fast16_t;
typedef _Atomic(uint_fast16_t)      atomic_uint_fast16_t;
typedef _Atomic(int_fast32_t)       atomic_int_fast32_t;
typedef _Atomic(uint_fast32_t)      atomic_uint_fast32_t;
typedef _Atomic(int_fast64_t)       atomic_int_fast64_t;
typedef _Atomic(uint_fast64_t)      atomic_uint_fast64_t;
typedef _Atomic(intptr_t)           atomic_intptr_t;
typedef _Atomic(uintptr_t)          atomic_uintptr_t;
typedef _Atomic(size_t)             atomic_size_t;
typedef _Atomic(ptrdiff_t)          atomic_ptrdiff_t;
typedef _Atomic(intmax_t)           atomic_intmax_t;
typedef _Atomic(uintmax_t)          atomic_uintmax_t;

#ifdef _MSC_VER
/* msvc has no __atomic builtins; cake generates code for them (see codegen.c) */
int __atomic_load_n();
void __atomic_store_n();
int __atomic_exchange_n();
_Bool __atomic_compare_exchange_n();
int __atomic_fetch_add();
int __atomic_fetch_sub();
int __atomic_fetch_and();
int __atomic_fetch_or();
int __atomic_fetch_xor();
void __atomic_thread_fence(int order);
void __atomic_signal_fence(int order);
_Bool __atomic_is_lock_free(size_t size, void* ptr);
#endif

/* the operations are the __atomic builtins, see include/builtins */
#define atomic_init(obj, value) __atomic_store_n(obj, value, __ATOMIC_RELAXED)
#define atomic_thread_fence(order) __atomic_thread_fence(order)
#define atomic_signal_fence(order) __atomic_signal_fence(order)
#define atomic_is_lock_free(obj) __atomic_is_lock_free(sizeof(*(obj)), (void*)(obj))

#define atomic_store(obj, desired) __atomic_store_n(obj, desired, __ATOMIC_SEQ_CST)
#define atomic_store_explicit(obj, desired, order) __atomic_store_n(obj, desired, order)
#define atomic_load(obj) __atomic_load_n(obj, __ATOMIC_SEQ_CST)
#define atomic_load_explicit(obj, order) __atomic_load_n(obj, order)
#define atomic_exchange(obj, desired) __atomic_exchange_n(obj, desired, __ATOMIC_SEQ_CST)
#define atomic_exchange_explicit(obj, desired, order) __atomic_exchange_n(obj, desired, order)

#define atomic_compare_exchange_strong(obj, expected, desired) \
    __atomic_compare_exchange_n(obj, expected, desired, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#define atomic_compare_exchange_strong_explicit(obj, expected, desired, success, failure) \
    __atomic_compare_exchange_n(obj, expected, desired, 0, success, failure)
#define atomic_compare_exchange_weak(obj, expected, desired) \
    __atomic_compare_exchange_n(obj, expected, desired, 1, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#define atomic_compare_exchange_weak_explicit(obj, expected, desired, success, failure) \
    __atomic_compare_exchange_n(obj, expected, desired, 1, success, failure)

#define atomic_fetch_add(obj, operand) __atomic_fetch_add(obj, operand, __ATOMIC_SEQ_CST)
#define atomic_fetch_add_explicit(obj, operand, order) __atomic_fetch_add(obj, operand, order)
#define atomic_fetch_sub(obj, operand) __atomic_fetch_sub(obj, operand, __ATOMIC_SEQ_CST)
#define atomic_fetch_sub_explicit(obj, operand, order) __atomic_fetch_sub(obj, operand, order)
#define atomic_fetch_or(obj, operand) __atomic_fetch_or(obj, operand, __ATOMIC_SEQ_CST)
#define atomic_fetch_or_explicit(obj, operand, order) __atomic_fetch_or(obj, operand, order)
#define atomic_fetch_xor(obj, operand) __atomic_fetch_xor(obj, operand, __ATOMIC_SEQ_CST)
#define atomic_fetch_xor_explicit(obj, operand, order) __atomic_fetch_xor(obj, operand, order)
#define atomic_fetch_and(obj, operand) __atomic_fetch_and(obj, operand, __ATOMIC_SEQ_CST)
#define atomic_fetch_and_explicit(obj, operand, order) __atomic_fetch_and(obj, operand, order)

typedef struct atomic_flag { atomic_bool _Value; } atomic_flag;

#define ATOMIC_FLAG_INIT { 0 }

#define atomic_flag_test_and_set(obj) __atomic_exchange_n(&(obj)->_Value, 1, __ATOMIC_SEQ_CST)
#define atomic_flag_test_and_set_explicit(obj, order) __atomic_exchange_n(&(obj)->_Value, 1, order)
#define atomic_flag_clear(obj) __atomic_store_n(&(obj)->_Value, 0, __ATOMIC_SEQ_CST)
#define atomic_flag_clear_explicit(obj, order) __atomic_store_n(&(obj)->_Value, 0, order)

#else
#include_next <stdatomic.h>
#endif
