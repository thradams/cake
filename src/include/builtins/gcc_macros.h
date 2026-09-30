
#define _CONSOLE
#define __STDC_NO_COMPLEX__  __STDC_NO_COMPLEX__
#define __STDC_NO_THREADS__   __STDC_NO_THREADS__
#define __STDC_NO_VLA__    __STDC_NO_VLA__
#define __linux__            1
#define __extension__        
#define __GNUC__             4
#define __GNUC_MINOR__       1
#define __STDC_HOSTED__      1
#define __STDC__             1
#ifdef __CAKE_TARGET_ARCH_X64
#define __x86_64__           1
#endif
#ifdef __CAKE_TARGET_ARCH_ARM64
#define __aarch64__          1
#define __AARCH64EL__        1
#define __ARM_ARCH           8
#define __ARM_64BIT_STATE    1
#define __LP64__             1
#define _LP64                1
#endif
#ifdef __CAKE_TARGET_ARCH_ARM32
#define __arm__              1
#define __ARMEL__            1
#define __ARM_EABI__         1
#define __ARM_PCS_VFP        1
#define __ARM_ARCH           __CAKE_ARM32_ARCH
#endif
#ifdef __CAKE_CHAR_UNSIGNED__
#define __CHAR_UNSIGNED__    1
#endif
#define __CHAR_BIT__         8
#define __SIZE_TYPE__        __CAKE_SIZE_TYPE__
#define __PTRDIFF_TYPE__     __CAKE_PTRDIFF_TYPE__
#define __WCHAR_TYPE__       __CAKE_WCHAR_TYPE__
#define __WINT_TYPE__        unsigned int
#ifdef __CAKE_TARGET_ARCH_ARM32
#define __INTMAX_TYPE__      long long int
#define __UINTMAX_TYPE__     long long unsigned int
#define __SIG_ATOMIC_TYPE__  int
#define __INT8_TYPE__        __CAKE_INT8_TYPE__
#define __INT16_TYPE__       __CAKE_INT16_TYPE__
#define __INT32_TYPE__       __CAKE_INT32_TYPE__
#define __INT64_TYPE__       __CAKE_INT64_TYPE__
#define __UINT8_TYPE__       __CAKE_UINT8_TYPE__
#define __UINT16_TYPE__      __CAKE_UINT16_TYPE__
#define __UINT32_TYPE__      __CAKE_UINT32_TYPE__
#define __UINT64_TYPE__      __CAKE_UINT64_TYPE__
#define __INT_LEAST8_TYPE__  signed char
#define __INT_LEAST16_TYPE__ short int
#define __INT_LEAST32_TYPE__ int
#define __INT_LEAST64_TYPE__ long long int
#define __UINT_LEAST8_TYPE__ unsigned char
#define __UINT_LEAST16_TYPE__ short unsigned int
#define __UINT_LEAST32_TYPE__ unsigned int
#define __UINT_LEAST64_TYPE__ long long unsigned int
#define __INT_FAST8_TYPE__    signed char
#define __INT_FAST16_TYPE__   int
#define __INT_FAST32_TYPE__   int
#define __INT_FAST64_TYPE__   long long int
#define __UINT_FAST8_TYPE__   unsigned char
#define __UINT_FAST16_TYPE__  unsigned int
#define __UINT_FAST32_TYPE__  unsigned int
#define __UINT_FAST64_TYPE__  long long unsigned int
#define __INTPTR_TYPE__       int
#define __UINTPTR_TYPE__      unsigned int
#else
#define __INTMAX_TYPE__      long int
#define __UINTMAX_TYPE__     long unsigned int
#define __SIG_ATOMIC_TYPE__  int
#define __INT8_TYPE__        __CAKE_INT8_TYPE__
#define __INT16_TYPE__       __CAKE_INT16_TYPE__
#define __INT32_TYPE__       __CAKE_INT32_TYPE__
#define __INT64_TYPE__       __CAKE_INT64_TYPE__
#define __UINT8_TYPE__       __CAKE_UINT8_TYPE__
#define __UINT16_TYPE__      __CAKE_UINT16_TYPE__
#define __UINT32_TYPE__      __CAKE_UINT32_TYPE__
#define __UINT64_TYPE__      __CAKE_UINT64_TYPE__
#define __INT_LEAST8_TYPE__  signed char
#define __INT_LEAST16_TYPE__ short int
#define __INT_LEAST32_TYPE__ int
#define __INT_LEAST64_TYPE__ long int
#define __UINT_LEAST8_TYPE__ unsigned char
#define __UINT_LEAST16_TYPE__ short unsigned int
#define __UINT_LEAST32_TYPE__ unsigned int
#define __UINT_LEAST64_TYPE__ long unsigned int
#define __INT_FAST8_TYPE__    signed char
#define __INT_FAST16_TYPE__   long int
#define __INT_FAST32_TYPE__   long int
#define __INT_FAST64_TYPE__   long int
#define __UINT_FAST8_TYPE__   unsigned char
#define __UINT_FAST16_TYPE__  long unsigned int
#define __UINT_FAST32_TYPE__  long unsigned int
#define __UINT_FAST64_TYPE__  long unsigned int
#define __INTPTR_TYPE__       long int
#define __UINTPTR_TYPE__      long unsigned int
#endif
#define __DBL_MAX__           ((double)1.79769313486231570814527423731704357e+308L)
#define __DBL_MIN__           ((double)2.22507385850720138309023271733240406e-308L)
#define __FLT_RADIX__         2
#define __FLT_EPSILON__       1.19209289550781250000000000000000000e-7F
#define __DBL_EPSILON__       ((double)2.22044604925031308084726333618164062e-16L)
#ifdef __CAKE_TARGET_ARCH_ARM64
#define __LDBL_EPSILON__      1.92592994438723585305597794258492732e-34L
#elif defined __CAKE_TARGET_ARCH_ARM32
#define __LDBL_EPSILON__      2.22044604925031308084726333618164062e-16L
#else
#define __LDBL_EPSILON__      1.08420217248550443400745280086994171e-19L
#endif
#define __DBL_DECIMAL_DIG__   17
#define __FLT_EVAL_METHOD__   0
#define __FLT_RADIX__         2
#define __DBL_MAX_EXP__       1024
#ifdef __CAKE_TARGET_ARCH_ARM64
#define __DECIMAL_DIG__       36
#elif defined __CAKE_TARGET_ARCH_ARM32
#define __DECIMAL_DIG__       17
#else
#define __DECIMAL_DIG__       21
#endif
#define __FLT_DECIMAL_DIG__   9
#define __FLT_MIN_10_EXP__    (-37)
#define __FLT_MIN__           1.17549435082228750796873653722224568e-38F
#define __FLT_MAX__           3.40282346638528859811704183484516925e+38F
#define __FLT_EPSILON__       1.19209289550781250000000000000000000e-7F
#define __FLT_DIG__           6
#define __FLT_MANT_DIG__      24
#define __FLT_MIN_EXP__       (-125)
#define __FLT_MAX_10_EXP__    38
#define __FLT_EVAL_METHOD__   0
#define __FLT_MAX_EXP__       128
#define __FLT_HAS_DENORM__    1
#define __SCHAR_MAX__         0x7f
#ifdef __CAKE_WCHAR_UNSIGNED__
#define __WCHAR_MAX__         0xffffffffU
#else
#define __WCHAR_MAX__         0x7fffffff
#endif
#define __SHRT_MAX__          0x7fff
#define __INT_MAX__           0x7fffffff
#ifdef __CAKE_TARGET_ARCH_ARM32
#define __LONG_MAX__          0x7fffffffL
#define __LONG_LONG_MAX__     0x7fffffffffffffffLL
#define __WINT_MAX__          0xffffffffU
#define __SIZE_MAX__          0xffffffffU
#define __PTRDIFF_MAX__       0x7fffffff
#define __INTMAX_MAX__        0x7fffffffffffffffLL
#define __UINTMAX_MAX__       0xffffffffffffffffULL
#define __SIG_ATOMIC_MAX__    0x7fffffff
#define __INT8_MAX__          0x7f
#define __INT16_MAX__         0x7fff
#define __INT32_MAX__         0x7fffffff
#define __INT64_MAX__         0x7fffffffffffffffLL
#define __UINT8_MAX__         0xff
#define __UINT16_MAX__        0xffff
#define __UINT32_MAX__        0xffffffffU
#define __UINT64_MAX__        0xffffffffffffffffULL
#define __INT_LEAST8_MAX__    0x7f
#define __INT_LEAST16_MAX__   0x7fff
#define __INT_LEAST32_MAX__   0x7fffffff
#define __INT_LEAST64_MAX__   0x7fffffffffffffffLL
#define __UINT_LEAST8_MAX__   0xff
#define __UINT_LEAST16_MAX__  0xffff
#define __UINT_LEAST32_MAX__  0xffffffffU
#define __UINT_LEAST64_MAX__  0xffffffffffffffffULL
#define __INT_FAST8_MAX__     0x7f
#define __INT_FAST16_MAX__    0x7fffffff
#define __INT_FAST32_MAX__    0x7fffffff
#define __INT_FAST64_MAX__    0x7fffffffffffffffLL
#define __UINT_FAST8_MAX__    0xff
#define __UINT_FAST16_MAX__   0xffffffffU
#define __UINT_FAST32_MAX__   0xffffffffU
#define __UINT_FAST64_MAX__   0xffffffffffffffffULL
#define __INTPTR_MAX__        0x7fffffff
#define __UINTPTR_MAX__       0xffffffffU
#ifdef __CAKE_WCHAR_UNSIGNED__
#define __WCHAR_MIN__        0U
#else
#define __WCHAR_MIN__        (-0x7fffffff - 1)
#endif
#define __WINT_MIN__         0U
#define __SIG_ATOMIC_MIN__ (-0x7fffffff - 1)
#define __INT8_C (-0x7fffffff - 1)
#define __SCHAR_WIDTH__ 8
#define __SHRT_WIDTH__ 16
#define __INT_WIDTH__ 32
#define __LONG_WIDTH__ 32
#define __LONG_LONG_WIDTH__ 64
#define __PTRDIFF_WIDTH__ 32
#define __SIG_ATOMIC_WIDTH__ 32
#define __SIZE_WIDTH__ 32
#define __WCHAR_WIDTH__ 32
#define __WINT_WIDTH__ 32
#define __INT_LEAST8_WIDTH__ 8
#define __INT_LEAST16_WIDTH__ 16
#define __INT_LEAST32_WIDTH__ 32
#define __INT_LEAST64_WIDTH__ 64
#define __INT_FAST8_WIDTH__ 8
#define __INT_FAST16_WIDTH__ 32
#define __INT_FAST32_WIDTH__ 32
#define __INT_FAST64_WIDTH__ 64
#define __INTPTR_WIDTH__ 32
#define __INTMAX_WIDTH__ 64
#else
#define __LONG_MAX__          0x7fffffffffffffffL
#define __LONG_LONG_MAX__     0x7fffffffffffffffLL
#define __WINT_MAX__          0xffffffffU
#define __SIZE_MAX__          0xffffffffffffffffUL
#define __PTRDIFF_MAX__       0x7fffffffffffffffL
#define __INTMAX_MAX__        0x7fffffffffffffffL
#define __UINTMAX_MAX__       0xffffffffffffffffUL
#define __SIG_ATOMIC_MAX__    0x7fffffff
#define __INT8_MAX__          0x7f
#define __INT16_MAX__         0x7fff
#define __INT32_MAX__         0x7fffffff
#define __INT64_MAX__         0x7fffffffffffffffL
#define __UINT8_MAX__         0xff
#define __UINT16_MAX__        0xffff
#define __UINT32_MAX__        0xffffffffU
#define __UINT64_MAX__        0xffffffffffffffffUL
#define __INT_LEAST8_MAX__    0x7f
#define __INT_LEAST16_MAX__   0x7fff
#define __INT_LEAST32_MAX__   0x7fffffff
#define __INT_LEAST64_MAX__   0x7fffffffffffffffL
#define __UINT_LEAST8_MAX__   0xff
#define __UINT_LEAST16_MAX__  0xffff
#define __UINT_LEAST32_MAX__  0xffffffffU
#define __UINT_LEAST64_MAX__  0xffffffffffffffffUL
#define __INT_FAST8_MAX__     0x7f
#define __INT_FAST16_MAX__    0x7fffffffffffffffL
#define __INT_FAST32_MAX__    0x7fffffffffffffffL
#define __INT_FAST64_MAX__    0x7fffffffffffffffL
#define __UINT_FAST8_MAX__    0xff
#define __UINT_FAST16_MAX__   0xffffffffffffffffUL
#define __UINT_FAST32_MAX__   0xffffffffffffffffUL
#define __UINT_FAST64_MAX__   0xffffffffffffffffUL
#define __INTPTR_MAX__        0x7fffffffffffffffL
#define __UINTPTR_MAX__       0xffffffffffffffffUL
#ifdef __CAKE_WCHAR_UNSIGNED__
#define __WCHAR_MIN__        0U
#else
#define __WCHAR_MIN__        (-0x7fffffff - 1)
#endif
#define __WINT_MIN__         0U
#define __SIG_ATOMIC_MIN__ (-0x7fffffff - 1)
#define __INT8_C (-0x7fffffff - 1)
#define __SCHAR_WIDTH__ 8
#define __SHRT_WIDTH__ 16
#define __INT_WIDTH__ 32
#define __LONG_WIDTH__ 64
#define __LONG_LONG_WIDTH__ 64
#define __PTRDIFF_WIDTH__ 64
#define __SIG_ATOMIC_WIDTH__ 32
#define __SIZE_WIDTH__ 64
#define __WCHAR_WIDTH__ 32
#define __WINT_WIDTH__ 32
#define __INT_LEAST8_WIDTH__ 8
#define __INT_LEAST16_WIDTH__ 16
#define __INT_LEAST32_WIDTH__ 32
#define __INT_LEAST64_WIDTH__ 64
#define __INT_FAST8_WIDTH__ 8
#define __INT_FAST16_WIDTH__ 64
#define __INT_FAST32_WIDTH__ 64
#define __INT_FAST64_WIDTH__ 64
#define __INTPTR_WIDTH__ 64
#define __INTMAX_WIDTH__ 64
#endif
#define __SIZEOF_INT__ __CAKE_SIZEOF_INT__
#define __SIZEOF_LONG__ __CAKE_SIZEOF_LONG__
#define __SIZEOF_LONG_LONG__ __CAKE_SIZEOF_LONG_LONG__
#define __SIZEOF_SHORT__ __CAKE_SIZEOF_SHORT__
#define __SIZEOF_POINTER__ __CAKE_SIZEOF_POINTER__
#define __SIZEOF_FLOAT__ __CAKE_SIZEOF_FLOAT__
#define __SIZEOF_DOUBLE__ __CAKE_SIZEOF_DOUBLE__
#define __SIZEOF_LONG_DOUBLE__ __CAKE_SIZEOF_LONG_DOUBLE__
#define __SIZEOF_SIZE_T__ __CAKE_SIZEOF_SIZE_T__
#define __SIZEOF_WCHAR_T__ __CAKE_SIZEOF_WCHAR_T__
#define __SIZEOF_WINT_T__ 4
#define __SIZEOF_PTRDIFF_T__ __CAKE_SIZEOF_PTRDIFF_T__


/* memory orders used by <stdatomic.h> and the __atomic/__c11_atomic builtins */
#define __ATOMIC_RELAXED 0
#define __ATOMIC_CONSUME 1
#define __ATOMIC_ACQUIRE 2
#define __ATOMIC_RELEASE 3
#define __ATOMIC_ACQ_REL 4
#define __ATOMIC_SEQ_CST 5

#define __GCC_ATOMIC_BOOL_LOCK_FREE 2
#define __GCC_ATOMIC_CHAR_LOCK_FREE 2
#define __GCC_ATOMIC_CHAR16_T_LOCK_FREE 2
#define __GCC_ATOMIC_CHAR32_T_LOCK_FREE 2
#define __GCC_ATOMIC_WCHAR_T_LOCK_FREE 2
#define __GCC_ATOMIC_SHORT_LOCK_FREE 2
#define __GCC_ATOMIC_INT_LOCK_FREE 2
#define __GCC_ATOMIC_LONG_LOCK_FREE 2
#define __GCC_ATOMIC_LLONG_LOCK_FREE 2
#define __GCC_ATOMIC_POINTER_LOCK_FREE 2
#define __GCC_ATOMIC_TEST_AND_SET_TRUEVAL 1
