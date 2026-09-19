/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
 *
 *  Internal header used by cake own headers (CAKE_HEADERS).
 *  It maps the target-dependent types to one spelling so that the other
 *  headers don't need to repeat the platform checks.
 *  Only macros are defined here; the typedefs (size_t, wchar_t, FILE ...)
 *  are made by the header that owns them.
 */

#pragma once

#if defined(_WIN64)

  #define __cake_size_t      unsigned long long
  #define __cake_ptrdiff_t   long long
  #define __cake_intptr_t    long long
  #define __cake_uintptr_t   unsigned long long
  #define __cake_wchar_t     unsigned short
  #define __cake_wint_t      unsigned short
  #define __cake_time_t      long long
  #define __cake_clock_t     long
  #define __cake_va_list     char*

#elif defined(_WIN32)

  #define __cake_size_t      unsigned int
  #define __cake_ptrdiff_t   int
  #define __cake_intptr_t    int
  #define __cake_uintptr_t   unsigned int
  #define __cake_wchar_t     unsigned short
  #define __cake_wint_t      unsigned short
  #define __cake_time_t      long long
  #define __cake_clock_t     long
  #define __cake_va_list     char*

#elif defined(__SIZE_TYPE__)

  /* gcc/clang like targets (linux, macOS): the compiler tells the types */
  #define __cake_size_t      __SIZE_TYPE__
  #define __cake_ptrdiff_t   __PTRDIFF_TYPE__
  #define __cake_intptr_t    __INTPTR_TYPE__
  #define __cake_uintptr_t   __UINTPTR_TYPE__
  #define __cake_wchar_t     __WCHAR_TYPE__
  #define __cake_wint_t      __WINT_TYPE__
  #define __cake_time_t      long
  #if defined(__APPLE__)
    #define __cake_clock_t   unsigned long
  #else
    #define __cake_clock_t   long
  #endif
  #define __cake_va_list     __builtin_va_list

#else

  /* small targets (ccu8, catalina) */
  #define __cake_size_t      unsigned int
  #define __cake_ptrdiff_t   int
  #define __cake_intptr_t    long
  #define __cake_uintptr_t   unsigned long
  #define __cake_wchar_t     unsigned short
  #define __cake_wint_t      unsigned short
  #define __cake_time_t      long
  #define __cake_clock_t     long
  #define __cake_va_list     char*

#endif

/*
  Types whose objects are created by the user program and filled by the
  library. Their size must match the real libc, so they are per platform.
*/
#if defined(_WIN32)
  /* same tag as corecrt.h, so both can be seen by one translation unit */
  typedef struct _Mbstatet { unsigned long _Wchar; unsigned short _Byte, _State; } __cake_mbstate_t;
  typedef long long __cake_fpos_t;
#elif defined(__APPLE__)
  typedef union { char __mbstate8[128]; long long __mbstateL; } __cake_mbstate_t;
  typedef long long __cake_fpos_t;
#elif defined(__linux__)
  typedef struct { int __count; union { unsigned int __wch; char __wchb[4]; } __value; } __cake_mbstate_t;
  typedef struct { long __pos; __cake_mbstate_t __state; } __cake_fpos_t;
#else
  typedef int __cake_mbstate_t;
  typedef long __cake_fpos_t;
#endif

/* fixed width types */
#if defined(__INT8_TYPE__)

  #define __cake_int8_t      __INT8_TYPE__
  #define __cake_int16_t     __INT16_TYPE__
  #define __cake_int32_t     __INT32_TYPE__
  #define __cake_int64_t     __INT64_TYPE__
  #define __cake_uint8_t     __UINT8_TYPE__
  #define __cake_uint16_t    __UINT16_TYPE__
  #define __cake_uint32_t    __UINT32_TYPE__
  #define __cake_uint64_t    __UINT64_TYPE__

#else

  #define __cake_int8_t      signed char
  #define __cake_int16_t     short
  #define __cake_int32_t     int
  #define __cake_int64_t     long long
  #define __cake_uint8_t     unsigned char
  #define __cake_uint16_t    unsigned short
  #define __cake_uint32_t    unsigned int
  #define __cake_uint64_t    unsigned long long

#endif

/*
  Suffix used by the 64 bits constants (INT64_MAX, INT64_C ...).
  It must produce the same type as __cake_int64_t.
*/
#if defined(__linux__) && defined(__SIZEOF_LONG__) && __SIZEOF_LONG__ == 8
  #define __cake_INT64_C(c)  c ## L
  #define __cake_UINT64_C(c) c ## UL
#else
  #define __cake_INT64_C(c)  c ## LL
  #define __cake_UINT64_C(c) c ## ULL
#endif

/* size of long, used by limits.h and stdint.h */
#if defined(__SIZEOF_LONG__)
  #define __cake_sizeof_long __SIZEOF_LONG__
#elif defined(_WIN32)
  #define __cake_sizeof_long 4
#elif defined(__CCU8__) || defined(__LCCU16__)
  #define __cake_sizeof_long 8
#else
  #define __cake_sizeof_long 4
#endif

/* size of int, used by limits.h */
#if defined(__SIZEOF_INT__)
  #define __cake_sizeof_int __SIZEOF_INT__
#elif defined(__CCU8__) || defined(__LCCU16__)
  #define __cake_sizeof_int 2
#else
  #define __cake_sizeof_int 4
#endif

/* size of pointer, used by stdint.h */
#if defined(__SIZEOF_POINTER__)
  #define __cake_sizeof_pointer __SIZEOF_POINTER__
#elif defined(_WIN64)
  #define __cake_sizeof_pointer 8
#else
  #define __cake_sizeof_pointer 4
#endif
