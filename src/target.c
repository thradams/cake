#include "target.h"
#include <limits.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#  define PLATFORM_NAME "windows"
#elif defined(__APPLE__) && defined(__MACH__)
#  define PLATFORM_NAME "apple"
#elif defined(__linux__)
#  define PLATFORM_NAME "linux"
#elif defined(__EMSCRIPTEN__)
#  define PLATFORM_NAME "emscripten"
#else
#error add new platform 
#endif



#if defined(__clang__)
#define COMPILER_NAME "clang"

#elif defined(__GNUC__) && !defined(__TINYC__) && !defined(__HLC__)
#define COMPILER_NAME "gcc"
#elif defined(_MSC_VER) && !defined(__TINYC__) && !defined(__HLC__)
#define COMPILER_NAME "msvc"
#elif defined(__INTEL_COMPILER)
#define COMPILER_NAME "intel"
#elif defined(__TINYC__)
#define COMPILER_NAME "tcc"
#elif defined(__HLC__)
#define COMPILER_NAME "hlc"
#else
#error add new compiler 
#endif


#if defined(__x86_64__) || defined(_M_X64)
#  define ARCH_NAME "X86_64"
#elif defined(__i386__) || defined(_M_IX86)
#  define ARCH_NAME "X86"

#elif defined(__aarch64__) || defined(_M_ARM64)
#  define ARCH_NAME "ARM64"
#elif defined(__arm__) || defined(_M_ARM)
#  define ARCH_NAME "ARM"
#elif defined(__wasm32__)
#  define ARCH_NAME "wasm32"
#elif defined(__wasm64__)
#  define ARCH_NAME "wasm64"
#elif defined(__wasm__)
#  define ARCH_NAME "wasm"
#else
#error add new architeture
#endif

int parse_target(const char* targetstr, enum target* target)
{
    if (strcmp(targetstr, "-target=" TARGET_X86_X64_GCC_STR) == 0)
    {
        *target = TARGET_X86_X64_GCC;
        return 0;
    }

    if (strcmp(targetstr, "-target=" TARGET_X64_MSVC_STR) == 0)
    {
        *target = TARGET_X64_MSVC;
        return 0;
    }

    if (strcmp(targetstr, "-target=" TARGET_X86_MSVC_STR) == 0)
    {
        *target = TARGET_X86_MSVC;
        return 0;
    }
    return 1; //error
}

void print_target_options()
{
    printf(""
            TARGET_X86_X64_GCC_STR " "
            TARGET_X86_MSVC_STR " "
            TARGET_X64_MSVC_STR " \n");
}

const char* target_to_string(enum target target)
{
    switch (target)
    {
    case TARGET_DEFAULT:
        return PLATFORM_NAME " " COMPILER_NAME  " " ARCH_NAME " (default)";

    case TARGET_X86_X64_GCC:
        return TARGET_X86_X64_GCC_STR;
    case TARGET_X86_MSVC:
        return TARGET_X86_MSVC_STR;
    case TARGET_X64_MSVC:
        return TARGET_X64_MSVC_STR;
    }
    return "";
}

int target_get_wchar_max(enum target target)
{
    switch (target)
    {
    case TARGET_DEFAULT:
#ifdef _WIN32
        return USHRT_MAX;
#else
        return INT_MAX;
#endif

    case TARGET_X86_X64_GCC:
        return INT_MAX;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        return USHRT_MAX;
    }

    assert(false);
    return 0;
}

long long target_get_signed_long_max(enum target target)
{
    switch (target)
    {
    case TARGET_DEFAULT:
        return LONG_MAX;

    case TARGET_X86_X64_GCC:
        return 0x7fffffffffffffffLL;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        return 2147483647LL;
    }

    assert(false);
    return 0;
}

long long target_get_signed_long_long_max(enum target target)
{
    switch (target)
    {
    case TARGET_DEFAULT:
        return LLONG_MAX;

    case TARGET_X86_X64_GCC:
        return 9223372036854775807LL;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        return 9223372036854775807LL;
    }

    assert(false);
    return 0;
}

unsigned long long target_get_unsigned_long_max(enum target target)
{
    switch (target)
    {
    case TARGET_DEFAULT:
        return ULONG_MAX;

    case TARGET_X86_X64_GCC:
        return 18446744073709551615ULL;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        return 0xffffffff;
    }

    assert(false);
    return 0;
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define CAKE_STANDARD_MACROS  \
"#define __CAKE__ 202311L\n"\
"#define __STDC_VERSION__ 202311L\n"\
"#define __FILE__ \"__FILE__\"\n"\
"#define __LINE__ 0\n"\
"#define __COUNTER__ 0\n"\
"#define _CONSOLE\n"\
"#define __STDC_OWNERSHIP__ 1\n" \
"#define __STDC_NO_ATOMICS__ " TOSTRING(__STDC_NO_ATOMICS__) "\n"\
"#define __STDC_NO_COMPLEX__  " TOSTRING(__STDC_NO_COMPLEX__) "\n"\
"#define __STDC_NO_THREADS__   " TOSTRING(__STDC_NO_THREADS__) "\n"\
"#define __STDC_NO_VLA__    " TOSTRING(__STDC_NO_VLA__) "\n"



#if 0

//PROGRAM TO GENERATE DEFINES

#include <stdio.h>
#include <stdlib.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)


int main()
{

    const char* text =
        /*some gcc stuff need to parse linux headers*/
        "#define __linux__\n"
        //see
        //https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html                
        "#define __x86_64__ " TOSTRING(__x86_64__) "\n"
        "#define __CHAR_BIT__ " TOSTRING(__CHAR_BIT__) "\n"
        "#define __SIZE_TYPE__ " TOSTRING(__SIZE_TYPE__) "\n"
        "#define __PTRDIFF_TYPE__ " TOSTRING(__PTRDIFF_TYPE__) "\n"
        "#define __WCHAR_TYPE__ " TOSTRING(__WCHAR_TYPE__) "\n"
        "#define __WINT_TYPE__ " TOSTRING(__WINT_TYPE__) "\n"
        "#define __INTMAX_TYPE__ " TOSTRING(__INTMAX_TYPE__) "\n"
        "#define __UINTMAX_TYPE__ " TOSTRING(__UINTMAX_TYPE__) "\n"
        "#define __SIG_ATOMIC_TYPE__ " TOSTRING(__SIG_ATOMIC_TYPE__) "\n"
        "#define __INT8_TYPE__ " TOSTRING(__INT8_TYPE__) "\n"
        "#define __INT16_TYPE__ " TOSTRING(__INT16_TYPE__) "\n"
        "#define __INT32_TYPE__ " TOSTRING(__INT32_TYPE__) "\n"
        "#define __INT64_TYPE__ " TOSTRING(__INT64_TYPE__) "\n"
        "#define __UINT8_TYPE__ " TOSTRING(__UINT8_TYPE__) "\n"
        "#define __UINT16_TYPE__ " TOSTRING(__UINT16_TYPE__) "\n"
        "#define __UINT32_TYPE__ " TOSTRING(__UINT32_TYPE__) "\n"
        "#define __UINT64_TYPE__ " TOSTRING(__UINT64_TYPE__) "\n"
        "#define __INT_LEAST8_TYPE__ " TOSTRING(__INT_LEAST8_TYPE__) "\n"
        "#define __INT_LEAST16_TYPE__ " TOSTRING(__INT_LEAST16_TYPE__) "\n"
        "#define __INT_LEAST32_TYPE__ " TOSTRING(__INT_LEAST32_TYPE__) "\n"
        "#define __INT_LEAST64_TYPE__ " TOSTRING(__INT_LEAST64_TYPE__) "\n"
        "#define __UINT_LEAST8_TYPE__ " TOSTRING(__UINT_LEAST8_TYPE__) "\n"
        "#define __UINT_LEAST16_TYPE__ " TOSTRING(__UINT_LEAST16_TYPE__) "\n"
        "#define __UINT_LEAST32_TYPE__ " TOSTRING(__UINT_LEAST32_TYPE__) "\n"
        "#define __UINT_LEAST64_TYPE__ " TOSTRING(__UINT_LEAST64_TYPE__) "\n"
        "#define __INT_FAST8_TYPE__ " TOSTRING(__INT_FAST8_TYPE__) "\n"
        "#define __INT_FAST16_TYPE__ " TOSTRING(__INT_FAST16_TYPE__) "\n"
        "#define __INT_FAST32_TYPE__ " TOSTRING(__INT_FAST32_TYPE__) "\n"
        "#define __INT_FAST64_TYPE__ " TOSTRING(__INT_FAST64_TYPE__) "\n"
        "#define __UINT_FAST8_TYPE__ " TOSTRING(__UINT_FAST8_TYPE__) "\n"
        "#define __UINT_FAST16_TYPE__ " TOSTRING(__UINT_FAST16_TYPE__) "\n"
        "#define __UINT_FAST32_TYPE__ " TOSTRING(__UINT_FAST32_TYPE__) "\n"
        "#define __UINT_FAST64_TYPE__ " TOSTRING(__UINT_FAST64_TYPE__) "\n"
        "#define __INTPTR_TYPE__ " TOSTRING(__INTPTR_TYPE__) "\n"
        "#define __UINTPTR_TYPE__ " TOSTRING(__UINTPTR_TYPE__) "\n"

        "#define __DBL_MAX__ " TOSTRING(__DBL_MAX__) "\n"
        "#define __DBL_MIN__ " TOSTRING(__DBL_MIN__) "\n"
        "#define __FLT_RADIX__ " TOSTRING(__FLT_RADIX__) "\n"
        "#define __FLT_EPSILON__ " TOSTRING(__FLT_EPSILON__) "\n"
        "#define __DBL_EPSILON__ " TOSTRING(__DBL_EPSILON__) "\n"
        "#define __LDBL_EPSILON__ " TOSTRING(__LDBL_EPSILON__) "\n"
        "#define __DBL_DECIMAL_DIG__ " TOSTRING(__DBL_DECIMAL_DIG__) "\n"
        "#define __FLT_EVAL_METHOD__ " TOSTRING(__FLT_EVAL_METHOD__) "\n"
        "#define __FLT_RADIX__ " TOSTRING(__FLT_RADIX__) "\n"

        // gcc -dM -E

        "#define __DBL_MAX_EXP__ " TOSTRING(__DBL_MAX_EXP__) "\n"
        "#define __DECIMAL_DIG__ " TOSTRING(__DECIMAL_DIG__) "\n"
        "#define __FLT_DECIMAL_DIG__ " TOSTRING(__FLT_DECIMAL_DIG__) "\n"


        "#define __FLT_MIN_10_EXP__ " TOSTRING(__FLT_MIN_10_EXP__) "\n"
        "#define __FLT_MIN__ " TOSTRING(__FLT_MIN__) "\n"
        "#define __FLT_MAX__ " TOSTRING(__FLT_MAX__) "\n"
        "#define __FLT_EPSILON__ " TOSTRING(__FLT_EPSILON__) "\n"
        "#define __FLT_DIG__ " TOSTRING(__FLT_DIG__) "\n"
        "#define __FLT_MANT_DIG__ " TOSTRING(__FLT_MANT_DIG__) "\n"
        "#define __FLT_MIN_EXP__ " TOSTRING(__FLT_MIN_EXP__) "\n"
        "#define __FLT_MAX_10_EXP__ " TOSTRING(__FLT_MAX_10_EXP__) "\n"
        "#define __FLT_ROUNDS__ " TOSTRING(__FLT_ROUNDS__) "\n"
        "#define __FLT_EVAL_METHOD__ " TOSTRING(__FLT_EVAL_METHOD__) "\n"
        "#define __FLT_HAS_SUBNORM__ " TOSTRING(__FLT_HAS_SUBNORM__) "\n"

        "#define __FLT_MAX_EXP__ " TOSTRING(__FLT_MAX_EXP__) "\n"
        "#define __FLT_HAS_DENORM__ " TOSTRING(__FLT_HAS_DENORM__) "\n"


        "#define __SCHAR_MAX__ " TOSTRING(__SCHAR_MAX__) "\n"
        "#define __WCHAR_MAX__ " TOSTRING(__WCHAR_MAX__) "\n"
        "#define __SHRT_MAX__ " TOSTRING(__SHRT_MAX__) "\n"
        "#define __INT_MAX__ " TOSTRING(__INT_MAX__) "\n"
        "#define __LONG_MAX__ " TOSTRING(__LONG_MAX__) "\n"
        "#define __LONG_LONG_MAX__ " TOSTRING(__LONG_LONG_MAX__) "\n"
        "#define __WINT_MAX__ " TOSTRING(__WINT_MAX__) "\n"
        "#define __SIZE_MAX__ " TOSTRING(__SIZE_MAX__) "\n"
        "#define __PTRDIFF_MAX__ " TOSTRING(__PTRDIFF_MAX__) "\n"
        "#define __INTMAX_MAX__ " TOSTRING(__INTMAX_MAX__) "\n"
        "#define __UINTMAX_MAX__ " TOSTRING(__UINTMAX_MAX__) "\n"
        "#define __SIG_ATOMIC_MAX__ " TOSTRING(__SIG_ATOMIC_MAX__) "\n"
        "#define __INT8_MAX__ " TOSTRING(__INT8_MAX__) "\n"
        "#define __INT16_MAX__ " TOSTRING(__INT16_MAX__) "\n"
        "#define __INT32_MAX__ " TOSTRING(__INT32_MAX__) "\n"
        "#define __INT64_MAX__ " TOSTRING(__INT64_MAX__) "\n"
        "#define __UINT8_MAX__ " TOSTRING(__UINT8_MAX__) "\n"
        "#define __UINT16_MAX__ " TOSTRING(__UINT16_MAX__) "\n"
        "#define __UINT32_MAX__ " TOSTRING(__UINT32_MAX__) "\n"
        "#define __UINT64_MAX__ " TOSTRING(__UINT64_MAX__) "\n"
        "#define __INT_LEAST8_MAX__ " TOSTRING(__INT_LEAST8_MAX__) "\n"
        "#define __INT_LEAST16_MAX__ " TOSTRING(__INT_LEAST16_MAX__) "\n"
        "#define __INT_LEAST32_MAX__ " TOSTRING(__INT_LEAST32_MAX__) "\n"
        "#define __INT_LEAST64_MAX__ " TOSTRING(__INT_LEAST64_MAX__) "\n"
        "#define __UINT_LEAST8_MAX__ " TOSTRING(__UINT_LEAST8_MAX__) "\n"
        "#define __UINT_LEAST16_MAX__ " TOSTRING(__UINT_LEAST16_MAX__) "\n"
        "#define __UINT_LEAST32_MAX__ " TOSTRING(__UINT_LEAST32_MAX__) "\n"
        "#define __UINT_LEAST64_MAX__ " TOSTRING(__UINT_LEAST64_MAX__) "\n"
        "#define __INT_FAST8_MAX__ " TOSTRING(__INT_FAST8_MAX__) "\n"
        "#define __INT_FAST16_MAX__ " TOSTRING(__INT_FAST16_MAX__) "\n"
        "#define __INT_FAST32_MAX__ " TOSTRING(__INT_FAST32_MAX__) "\n"
        "#define __INT_FAST64_MAX__ " TOSTRING(__INT_FAST64_MAX__) "\n"
        "#define __UINT_FAST8_MAX__ " TOSTRING(__UINT_FAST8_MAX__) "\n"
        "#define __UINT_FAST16_MAX__ " TOSTRING(__UINT_FAST16_MAX__) "\n"
        "#define __UINT_FAST32_MAX__ " TOSTRING(__UINT_FAST32_MAX__) "\n"
        "#define __UINT_FAST64_MAX__ " TOSTRING(__UINT_FAST64_MAX__) "\n"
        "#define __INTPTR_MAX__ " TOSTRING(__INTPTR_MAX__) "\n"
        "#define __UINTPTR_MAX__ " TOSTRING(__UINTPTR_MAX__) "\n"
        "#define __WCHAR_MIN__ " TOSTRING(__WCHAR_MIN__) "\n"
        "#define __WINT_MIN__ " TOSTRING(__WINT_MIN__) "\n"
        "#define __SIG_ATOMIC_MIN__ " TOSTRING(__SIG_ATOMIC_MIN__) "\n"

        "#define __INT8_C " TOSTRING(__SIG_ATOMIC_MIN__) "\n"
        "#define __INT16_C " TOSTRING(__INT16_C) "\n"
        "#define __INT32_C " TOSTRING(__INT32_C) "\n"
        "#define __INT64_C " TOSTRING(__INT64_C) "\n"
        "#define __UINT8_C " TOSTRING(__UINT8_C) "\n"
        "#define __UINT16_C " TOSTRING(__UINT16_C) "\n"
        "#define __UINT32_C " TOSTRING(__UINT32_C) "\n"
        "#define __UINT64_C " TOSTRING(__UINT64_C) "\n"
        "#define __INTMAX_C " TOSTRING(__INTMAX_C) "\n"
        "#define __UINTMAX_C " TOSTRING(__UINTMAX_C) "\n"

        "#define __SCHAR_WIDTH__ " TOSTRING(__SCHAR_WIDTH__) "\n"
        "#define __SHRT_WIDTH__ " TOSTRING(__SHRT_WIDTH__) "\n"
        "#define __INT_WIDTH__ " TOSTRING(__INT_WIDTH__) "\n"
        "#define __LONG_WIDTH__ " TOSTRING(__LONG_WIDTH__) "\n"
        "#define __LONG_LONG_WIDTH__ " TOSTRING(__LONG_LONG_WIDTH__) "\n"
        "#define __PTRDIFF_WIDTH__ " TOSTRING(__PTRDIFF_WIDTH__) "\n"
        "#define __SIG_ATOMIC_WIDTH__ " TOSTRING(__SIG_ATOMIC_WIDTH__) "\n"
        "#define __SIZE_WIDTH__ " TOSTRING(__SIZE_WIDTH__) "\n"
        "#define __WCHAR_WIDTH__ " TOSTRING(__WCHAR_WIDTH__) "\n"
        "#define __WINT_WIDTH__ " TOSTRING(__WINT_WIDTH__) "\n"
        "#define __INT_LEAST8_WIDTH__ " TOSTRING(__INT_LEAST8_WIDTH__) "\n"
        "#define __INT_LEAST16_WIDTH__ " TOSTRING(__INT_LEAST16_WIDTH__) "\n"
        "#define __INT_LEAST32_WIDTH__ " TOSTRING(__INT_LEAST32_WIDTH__) "\n"
        "#define __INT_LEAST64_WIDTH__ " TOSTRING(__INT_LEAST64_WIDTH__) "\n"
        "#define __INT_FAST8_WIDTH__ " TOSTRING(__INT_FAST8_WIDTH__) "\n"
        "#define __INT_FAST16_WIDTH__ " TOSTRING(__INT_FAST16_WIDTH__) "\n"
        "#define __INT_FAST32_WIDTH__ " TOSTRING(__INT_FAST32_WIDTH__) "\n"
        "#define __INT_FAST64_WIDTH__ " TOSTRING(__INT_FAST64_WIDTH__) "\n"
        "#define __INTPTR_WIDTH__ " TOSTRING(__INTPTR_WIDTH__) "\n"
        "#define __INTMAX_WIDTH__ " TOSTRING(__INTMAX_WIDTH__) "\n"
        "#define __SIZEOF_INT__ " TOSTRING(__SIZEOF_INT__) "\n"
        "#define __SIZEOF_LONG__ " TOSTRING(__SIZEOF_LONG__) "\n"
        "#define __SIZEOF_LONG_LONG__ " TOSTRING(__SIZEOF_LONG_LONG__) "\n"
        "#define __SIZEOF_SHORT__ " TOSTRING(__SIZEOF_SHORT__) "\n"
        "#define __SIZEOF_POINTER__ " TOSTRING(__SIZEOF_POINTER__) "\n"
        "#define __SIZEOF_FLOAT__ " TOSTRING(__SIZEOF_FLOAT__) "\n"
        "#define __SIZEOF_DOUBLE__ " TOSTRING(__SIZEOF_DOUBLE__) "\n"
        "#define __SIZEOF_LONG_DOUBLE__ " TOSTRING(__SIZEOF_LONG_DOUBLE__) "\n"
        "#define __SIZEOF_SIZE_T__ " TOSTRING(__SIZEOF_SIZE_T__) "\n"
        "#define __SIZEOF_WCHAR_T__ " TOSTRING(__SIZEOF_WCHAR_T__) "\n"
        "#define __SIZEOF_WINT_T__ " TOSTRING(__SIZEOF_WINT_T__) "\n"
        "#define __SIZEOF_PTRDIFF_T__ " TOSTRING(__SIZEOF_PTRDIFF_T__) "\n";
    printf("%s", text);
}

#endif

const char* TARGET_X86_X64_GCC_PREDEFINED_MACROS =
#ifdef __EMSCRIPTEN__
//include dir on emscripten
"#pragma dir \"c:/\"\n"
#endif

CAKE_STANDARD_MACROS
"#define __linux__\n"
"#define __x86_64__ 1\n"
"#define __CHAR_BIT__ 8\n"
"#define __SIZE_TYPE__ long unsigned int\n"
"#define __PTRDIFF_TYPE__ long int\n"
"#define __WCHAR_TYPE__ int\n"
"#define __WINT_TYPE__ unsigned int\n"
"#define __INTMAX_TYPE__ long int\n"
"#define __UINTMAX_TYPE__ long unsigned int\n"
"#define __SIG_ATOMIC_TYPE__ int\n"
"#define __INT8_TYPE__ signed char\n"
"#define __INT16_TYPE__ short int\n"
"#define __INT32_TYPE__ int\n"
"#define __INT64_TYPE__ long int\n"
"#define __UINT8_TYPE__ unsigned char\n"
"#define __UINT16_TYPE__ short unsigned int\n"
"#define __UINT32_TYPE__ unsigned int\n"
"#define __UINT64_TYPE__ long unsigned int\n"
"#define __INT_LEAST8_TYPE__ signed char\n"
"#define __INT_LEAST16_TYPE__ short int\n"
"#define __INT_LEAST32_TYPE__ int\n"
"#define __INT_LEAST64_TYPE__ long int\n"
"#define __UINT_LEAST8_TYPE__ unsigned char\n"
"#define __UINT_LEAST16_TYPE__ short unsigned int\n"
"#define __UINT_LEAST32_TYPE__ unsigned int\n"
"#define __UINT_LEAST64_TYPE__ long unsigned int\n"
"#define __INT_FAST8_TYPE__ signed char\n"
"#define __INT_FAST16_TYPE__ long int\n"
"#define __INT_FAST32_TYPE__ long int\n"
"#define __INT_FAST64_TYPE__ long int\n"
"#define __UINT_FAST8_TYPE__ unsigned char\n"
"#define __UINT_FAST16_TYPE__ long unsigned int\n"
"#define __UINT_FAST32_TYPE__ long unsigned int\n"
"#define __UINT_FAST64_TYPE__ long unsigned int\n"
"#define __INTPTR_TYPE__ long int\n"
"#define __UINTPTR_TYPE__ long unsigned int\n"
"#define __DBL_MAX__ ((double)1.79769313486231570814527423731704357e+308L)\n"
"#define __DBL_MIN__ ((double)2.22507385850720138309023271733240406e-308L)\n"
"#define __FLT_RADIX__ 2\n"
"#define __FLT_EPSILON__ 1.19209289550781250000000000000000000e-7F\n"
"#define __DBL_EPSILON__ ((double)2.22044604925031308084726333618164062e-16L)\n"
"#define __LDBL_EPSILON__ 1.08420217248550443400745280086994171e-19L\n"
"#define __DBL_DECIMAL_DIG__ 17\n"
"#define __FLT_EVAL_METHOD__ 0\n"
"#define __FLT_RADIX__ 2\n"
"#define __DBL_MAX_EXP__ 1024\n"
"#define __DECIMAL_DIG__ 21\n"
"#define __FLT_DECIMAL_DIG__ 9\n"
"#define __FLT_MIN_10_EXP__ (-37)\n"
"#define __FLT_MIN__ 1.17549435082228750796873653722224568e-38F\n"
"#define __FLT_MAX__ 3.40282346638528859811704183484516925e+38F\n"
"#define __FLT_EPSILON__ 1.19209289550781250000000000000000000e-7F\n"
"#define __FLT_DIG__ 6\n"
"#define __FLT_MANT_DIG__ 24\n"
"#define __FLT_MIN_EXP__ (-125)\n"
"#define __FLT_MAX_10_EXP__ 38\n"
"#define __FLT_ROUNDS__ __FLT_ROUNDS__\n"
"#define __FLT_EVAL_METHOD__ 0\n"
"#define __FLT_HAS_SUBNORM__ __FLT_HAS_SUBNORM__\n"
"#define __FLT_MAX_EXP__ 128\n"
"#define __FLT_HAS_DENORM__ 1\n"
"#define __SCHAR_MAX__ 0x7f\n"
"#define __WCHAR_MAX__ 0x7fffffff\n"
"#define __SHRT_MAX__ 0x7fff\n"
"#define __INT_MAX__ 0x7fffffff\n"
"#define __LONG_MAX__ 0x7fffffffffffffffL\n"
"#define __LONG_LONG_MAX__ 0x7fffffffffffffffLL\n"
"#define __WINT_MAX__ 0xffffffffU\n"
"#define __SIZE_MAX__ 0xffffffffffffffffUL\n"
"#define __PTRDIFF_MAX__ 0x7fffffffffffffffL\n"
"#define __INTMAX_MAX__ 0x7fffffffffffffffL\n"
"#define __UINTMAX_MAX__ 0xffffffffffffffffUL\n"
"#define __SIG_ATOMIC_MAX__ 0x7fffffff\n"
"#define __INT8_MAX__ 0x7f\n"
"#define __INT16_MAX__ 0x7fff\n"
"#define __INT32_MAX__ 0x7fffffff\n"
"#define __INT64_MAX__ 0x7fffffffffffffffL\n"
"#define __UINT8_MAX__ 0xff\n"
"#define __UINT16_MAX__ 0xffff\n"
"#define __UINT32_MAX__ 0xffffffffU\n"
"#define __UINT64_MAX__ 0xffffffffffffffffUL\n"
"#define __INT_LEAST8_MAX__ 0x7f\n"
"#define __INT_LEAST16_MAX__ 0x7fff\n"
"#define __INT_LEAST32_MAX__ 0x7fffffff\n"
"#define __INT_LEAST64_MAX__ 0x7fffffffffffffffL\n"
"#define __UINT_LEAST8_MAX__ 0xff\n"
"#define __UINT_LEAST16_MAX__ 0xffff\n"
"#define __UINT_LEAST32_MAX__ 0xffffffffU\n"
"#define __UINT_LEAST64_MAX__ 0xffffffffffffffffUL\n"
"#define __INT_FAST8_MAX__ 0x7f\n"
"#define __INT_FAST16_MAX__ 0x7fffffffffffffffL\n"
"#define __INT_FAST32_MAX__ 0x7fffffffffffffffL\n"
"#define __INT_FAST64_MAX__ 0x7fffffffffffffffL\n"
"#define __UINT_FAST8_MAX__ 0xff\n"
"#define __UINT_FAST16_MAX__ 0xffffffffffffffffUL\n"
"#define __UINT_FAST32_MAX__ 0xffffffffffffffffUL\n"
"#define __UINT_FAST64_MAX__ 0xffffffffffffffffUL\n"
"#define __INTPTR_MAX__ 0x7fffffffffffffffL\n"
"#define __UINTPTR_MAX__ 0xffffffffffffffffUL\n"
"#define __WCHAR_MIN__ (-0x7fffffff - 1)\n"
"#define __WINT_MIN__ 0U\n"
"#define __SIG_ATOMIC_MIN__ (-0x7fffffff - 1)\n"
"#define __INT8_C (-0x7fffffff - 1)\n"
"#define __INT16_C __INT16_C\n"
"#define __INT32_C __INT32_C\n"
"#define __INT64_C __INT64_C\n"
"#define __UINT8_C __UINT8_C\n"
"#define __UINT16_C __UINT16_C\n"
"#define __UINT32_C __UINT32_C\n"
"#define __UINT64_C __UINT64_C\n"
"#define __INTMAX_C __INTMAX_C\n"
"#define __UINTMAX_C __UINTMAX_C\n"
"#define __SCHAR_WIDTH__ 8\n"
"#define __SHRT_WIDTH__ 16\n"
"#define __INT_WIDTH__ 32\n"
"#define __LONG_WIDTH__ 64\n"
"#define __LONG_LONG_WIDTH__ 64\n"
"#define __PTRDIFF_WIDTH__ 64\n"
"#define __SIG_ATOMIC_WIDTH__ 32\n"
"#define __SIZE_WIDTH__ 64\n"
"#define __WCHAR_WIDTH__ 32\n"
"#define __WINT_WIDTH__ 32\n"
"#define __INT_LEAST8_WIDTH__ 8\n"
"#define __INT_LEAST16_WIDTH__ 16\n"
"#define __INT_LEAST32_WIDTH__ 32\n"
"#define __INT_LEAST64_WIDTH__ 64\n"
"#define __INT_FAST8_WIDTH__ 8\n"
"#define __INT_FAST16_WIDTH__ 64\n"
"#define __INT_FAST32_WIDTH__ 64\n"
"#define __INT_FAST64_WIDTH__ 64\n"
"#define __INTPTR_WIDTH__ 64\n"
"#define __INTMAX_WIDTH__ 64\n"
"#define __SIZEOF_INT__ 4\n"
"#define __SIZEOF_LONG__ 8\n"
"#define __SIZEOF_LONG_LONG__ 8\n"
"#define __SIZEOF_SHORT__ 2\n"
"#define __SIZEOF_POINTER__ 8\n"
"#define __SIZEOF_FLOAT__ 4\n"
"#define __SIZEOF_DOUBLE__ 8\n"
"#define __SIZEOF_LONG_DOUBLE__ 16\n"
"#define __SIZEOF_SIZE_T__ 8\n"
"#define __SIZEOF_WCHAR_T__ 4\n"
"#define __SIZEOF_WINT_T__ 4\n"
"#define __SIZEOF_PTRDIFF_T__ 8\n";


const char* TARGET_DEFAULT_PREDEFINED_MACROS =
CAKE_STANDARD_MACROS

#ifdef __EMSCRIPTEN__
//include dir on emscripten
"#pragma dir \"c:/\"\n"
#endif

#ifdef _WIN32

//see
//https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?_View=msvc-170
"#define _WIN32 " TOSTRING(_WIN32) "\n"


#ifdef _WIN64
"#define _WIN64 " TOSTRING(_WIN64) "\n"
#endif

"#define _INTEGRAL_MAX_BITS " TOSTRING(_INTEGRAL_MAX_BITS) "\n" /*Use of __int64 should be conditional on the predefined macro _INTEGRAL_MAX_BITS*/

"#define _MSC_VER " TOSTRING(_MSC_VER) "\n"
"#define _M_IX86 "  TOSTRING(_M_IX86) "\n"
"#define __pragma(a)\n"

#endif

#ifdef __linux__

//https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
    /*some gcc stuff need to parse linux headers*/
    "#define __linux__\n"
    //see
    //https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html                
    "#define __x86_64__ " TOSTRING(__x86_64__) "\n"
    "#define __CHAR_BIT__ " TOSTRING(__CHAR_BIT__) "\n"
    "#define __SIZE_TYPE__ " TOSTRING(__SIZE_TYPE__) "\n"
    "#define __PTRDIFF_TYPE__ " TOSTRING(__PTRDIFF_TYPE__) "\n"
    "#define __WCHAR_TYPE__ " TOSTRING(__WCHAR_TYPE__) "\n"
    "#define __WINT_TYPE__ " TOSTRING(__WINT_TYPE__) "\n"
    "#define __INTMAX_TYPE__ " TOSTRING(__INTMAX_TYPE__) "\n"
    "#define __UINTMAX_TYPE__ " TOSTRING(__UINTMAX_TYPE__) "\n"
    "#define __SIG_ATOMIC_TYPE__ " TOSTRING(__SIG_ATOMIC_TYPE__) "\n"
    "#define __INT8_TYPE__ " TOSTRING(__INT8_TYPE__) "\n"
    "#define __INT16_TYPE__ " TOSTRING(__INT16_TYPE__) "\n"
    "#define __INT32_TYPE__ " TOSTRING(__INT32_TYPE__) "\n"
    "#define __INT64_TYPE__ " TOSTRING(__INT64_TYPE__) "\n"
    "#define __UINT8_TYPE__ " TOSTRING(__UINT8_TYPE__) "\n"
    "#define __UINT16_TYPE__ " TOSTRING(__UINT16_TYPE__) "\n"
    "#define __UINT32_TYPE__ " TOSTRING(__UINT32_TYPE__) "\n"
    "#define __UINT64_TYPE__ " TOSTRING(__UINT64_TYPE__) "\n"
    "#define __INT_LEAST8_TYPE__ " TOSTRING(__INT_LEAST8_TYPE__) "\n"
    "#define __INT_LEAST16_TYPE__ " TOSTRING(__INT_LEAST16_TYPE__) "\n"
    "#define __INT_LEAST32_TYPE__ " TOSTRING(__INT_LEAST32_TYPE__) "\n"
    "#define __INT_LEAST64_TYPE__ " TOSTRING(__INT_LEAST64_TYPE__) "\n"
    "#define __UINT_LEAST8_TYPE__ " TOSTRING(__UINT_LEAST8_TYPE__) "\n"
    "#define __UINT_LEAST16_TYPE__ " TOSTRING(__UINT_LEAST16_TYPE__) "\n"
    "#define __UINT_LEAST32_TYPE__ " TOSTRING(__UINT_LEAST32_TYPE__) "\n"
    "#define __UINT_LEAST64_TYPE__ " TOSTRING(__UINT_LEAST64_TYPE__) "\n"
    "#define __INT_FAST8_TYPE__ " TOSTRING(__INT_FAST8_TYPE__) "\n"
    "#define __INT_FAST16_TYPE__ " TOSTRING(__INT_FAST16_TYPE__) "\n"
    "#define __INT_FAST32_TYPE__ " TOSTRING(__INT_FAST32_TYPE__) "\n"
    "#define __INT_FAST64_TYPE__ " TOSTRING(__INT_FAST64_TYPE__) "\n"
    "#define __UINT_FAST8_TYPE__ " TOSTRING(__UINT_FAST8_TYPE__) "\n"
    "#define __UINT_FAST16_TYPE__ " TOSTRING(__UINT_FAST16_TYPE__) "\n"
    "#define __UINT_FAST32_TYPE__ " TOSTRING(__UINT_FAST32_TYPE__) "\n"
    "#define __UINT_FAST64_TYPE__ " TOSTRING(__UINT_FAST64_TYPE__) "\n"
    "#define __INTPTR_TYPE__ " TOSTRING(__INTPTR_TYPE__) "\n"
    "#define __UINTPTR_TYPE__ " TOSTRING(__UINTPTR_TYPE__) "\n"

    "#define __DBL_MAX__ " TOSTRING(__DBL_MAX__) "\n"
    "#define __DBL_MIN__ " TOSTRING(__DBL_MIN__) "\n"
    "#define __FLT_RADIX__ " TOSTRING(__FLT_RADIX__) "\n"
    "#define __FLT_EPSILON__ " TOSTRING(__FLT_EPSILON__) "\n"
    "#define __DBL_EPSILON__ " TOSTRING(__DBL_EPSILON__) "\n"
    "#define __LDBL_EPSILON__ " TOSTRING(__LDBL_EPSILON__) "\n"
    "#define __DBL_DECIMAL_DIG__ " TOSTRING(__DBL_DECIMAL_DIG__) "\n"
    "#define __FLT_EVAL_METHOD__ " TOSTRING(__FLT_EVAL_METHOD__) "\n"
    "#define __FLT_RADIX__ " TOSTRING(__FLT_RADIX__) "\n"

    "#define __DBL_MAX_EXP__ " TOSTRING(__DBL_MAX_EXP__) "\n"
    "#define __DECIMAL_DIG__ " TOSTRING(__DECIMAL_DIG__) "\n"
    "#define __FLT_DECIMAL_DIG__ " TOSTRING(__FLT_DECIMAL_DIG__) "\n"


    "#define __FLT_MIN_10_EXP__ " TOSTRING(__FLT_MIN_10_EXP__) "\n"
    "#define __FLT_MIN__ " TOSTRING(__FLT_MIN__) "\n"
    "#define __FLT_MAX__ " TOSTRING(__FLT_MAX__) "\n"
    "#define __FLT_EPSILON__ " TOSTRING(__FLT_EPSILON__) "\n"
    "#define __FLT_DIG__ " TOSTRING(__FLT_DIG__) "\n"
    "#define __FLT_MANT_DIG__ " TOSTRING(__FLT_MANT_DIG__) "\n"
    "#define __FLT_MIN_EXP__ " TOSTRING(__FLT_MIN_EXP__) "\n"
    "#define __FLT_MAX_10_EXP__ " TOSTRING(__FLT_MAX_10_EXP__) "\n"
    "#define __FLT_ROUNDS__ " TOSTRING(__FLT_ROUNDS__) "\n"
    "#define __FLT_EVAL_METHOD__ " TOSTRING(__FLT_EVAL_METHOD__) "\n"
    "#define __FLT_HAS_SUBNORM__ " TOSTRING(__FLT_HAS_SUBNORM__) "\n"

    "#define __FLT_MAX_EXP__ " TOSTRING(__FLT_MAX_EXP__) "\n"
    "#define __FLT_HAS_DENORM__ " TOSTRING(__FLT_HAS_DENORM__) "\n"


    "#define __SCHAR_MAX__ " TOSTRING(__SCHAR_MAX__) "\n"
    "#define __WCHAR_MAX__ " TOSTRING(__WCHAR_MAX__) "\n"
    "#define __SHRT_MAX__ " TOSTRING(__SHRT_MAX__) "\n"
    "#define __INT_MAX__ " TOSTRING(__INT_MAX__) "\n"
    "#define __LONG_MAX__ " TOSTRING(__LONG_MAX__) "\n"
    "#define __LONG_LONG_MAX__ " TOSTRING(__LONG_LONG_MAX__) "\n"
    "#define __WINT_MAX__ " TOSTRING(__WINT_MAX__) "\n"
    "#define __SIZE_MAX__ " TOSTRING(__SIZE_MAX__) "\n"
    "#define __PTRDIFF_MAX__ " TOSTRING(__PTRDIFF_MAX__) "\n"
    "#define __INTMAX_MAX__ " TOSTRING(__INTMAX_MAX__) "\n"
    "#define __UINTMAX_MAX__ " TOSTRING(__UINTMAX_MAX__) "\n"
    "#define __SIG_ATOMIC_MAX__ " TOSTRING(__SIG_ATOMIC_MAX__) "\n"
    "#define __INT8_MAX__ " TOSTRING(__INT8_MAX__) "\n"
    "#define __INT16_MAX__ " TOSTRING(__INT16_MAX__) "\n"
    "#define __INT32_MAX__ " TOSTRING(__INT32_MAX__) "\n"
    "#define __INT64_MAX__ " TOSTRING(__INT64_MAX__) "\n"
    "#define __UINT8_MAX__ " TOSTRING(__UINT8_MAX__) "\n"
    "#define __UINT16_MAX__ " TOSTRING(__UINT16_MAX__) "\n"
    "#define __UINT32_MAX__ " TOSTRING(__UINT32_MAX__) "\n"
    "#define __UINT64_MAX__ " TOSTRING(__UINT64_MAX__) "\n"
    "#define __INT_LEAST8_MAX__ " TOSTRING(__INT_LEAST8_MAX__) "\n"
    "#define __INT_LEAST16_MAX__ " TOSTRING(__INT_LEAST16_MAX__) "\n"
    "#define __INT_LEAST32_MAX__ " TOSTRING(__INT_LEAST32_MAX__) "\n"
    "#define __INT_LEAST64_MAX__ " TOSTRING(__INT_LEAST64_MAX__) "\n"
    "#define __UINT_LEAST8_MAX__ " TOSTRING(__UINT_LEAST8_MAX__) "\n"
    "#define __UINT_LEAST16_MAX__ " TOSTRING(__UINT_LEAST16_MAX__) "\n"
    "#define __UINT_LEAST32_MAX__ " TOSTRING(__UINT_LEAST32_MAX__) "\n"
    "#define __UINT_LEAST64_MAX__ " TOSTRING(__UINT_LEAST64_MAX__) "\n"
    "#define __INT_FAST8_MAX__ " TOSTRING(__INT_FAST8_MAX__) "\n"
    "#define __INT_FAST16_MAX__ " TOSTRING(__INT_FAST16_MAX__) "\n"
    "#define __INT_FAST32_MAX__ " TOSTRING(__INT_FAST32_MAX__) "\n"
    "#define __INT_FAST64_MAX__ " TOSTRING(__INT_FAST64_MAX__) "\n"
    "#define __UINT_FAST8_MAX__ " TOSTRING(__UINT_FAST8_MAX__) "\n"
    "#define __UINT_FAST16_MAX__ " TOSTRING(__UINT_FAST16_MAX__) "\n"
    "#define __UINT_FAST32_MAX__ " TOSTRING(__UINT_FAST32_MAX__) "\n"
    "#define __UINT_FAST64_MAX__ " TOSTRING(__UINT_FAST64_MAX__) "\n"
    "#define __INTPTR_MAX__ " TOSTRING(__INTPTR_MAX__) "\n"
    "#define __UINTPTR_MAX__ " TOSTRING(__UINTPTR_MAX__) "\n"
    "#define __WCHAR_MIN__ " TOSTRING(__WCHAR_MIN__) "\n"
    "#define __WINT_MIN__ " TOSTRING(__WINT_MIN__) "\n"
    "#define __SIG_ATOMIC_MIN__ " TOSTRING(__SIG_ATOMIC_MIN__) "\n"

    "#define __INT8_C " TOSTRING(__SIG_ATOMIC_MIN__) "\n"
    "#define __INT16_C " TOSTRING(__INT16_C) "\n"
    "#define __INT32_C " TOSTRING(__INT32_C) "\n"
    "#define __INT64_C " TOSTRING(__INT64_C) "\n"
    "#define __UINT8_C " TOSTRING(__UINT8_C) "\n"
    "#define __UINT16_C " TOSTRING(__UINT16_C) "\n"
    "#define __UINT32_C " TOSTRING(__UINT32_C) "\n"
    "#define __UINT64_C " TOSTRING(__UINT64_C) "\n"
    "#define __INTMAX_C " TOSTRING(__INTMAX_C) "\n"
    "#define __UINTMAX_C " TOSTRING(__UINTMAX_C) "\n"

    "#define __SCHAR_WIDTH__ " TOSTRING(__SCHAR_WIDTH__) "\n"
    "#define __SHRT_WIDTH__ " TOSTRING(__SHRT_WIDTH__) "\n"
    "#define __INT_WIDTH__ " TOSTRING(__INT_WIDTH__) "\n"
    "#define __LONG_WIDTH__ " TOSTRING(__LONG_WIDTH__) "\n"
    "#define __LONG_LONG_WIDTH__ " TOSTRING(__LONG_LONG_WIDTH__) "\n"
    "#define __PTRDIFF_WIDTH__ " TOSTRING(__PTRDIFF_WIDTH__) "\n"
    "#define __SIG_ATOMIC_WIDTH__ " TOSTRING(__SIG_ATOMIC_WIDTH__) "\n"
    "#define __SIZE_WIDTH__ " TOSTRING(__SIZE_WIDTH__) "\n"
    "#define __WCHAR_WIDTH__ " TOSTRING(__WCHAR_WIDTH__) "\n"
    "#define __WINT_WIDTH__ " TOSTRING(__WINT_WIDTH__) "\n"
    "#define __INT_LEAST8_WIDTH__ " TOSTRING(__INT_LEAST8_WIDTH__) "\n"
    "#define __INT_LEAST16_WIDTH__ " TOSTRING(__INT_LEAST16_WIDTH__) "\n"
    "#define __INT_LEAST32_WIDTH__ " TOSTRING(__INT_LEAST32_WIDTH__) "\n"
    "#define __INT_LEAST64_WIDTH__ " TOSTRING(__INT_LEAST64_WIDTH__) "\n"
    "#define __INT_FAST8_WIDTH__ " TOSTRING(__INT_FAST8_WIDTH__) "\n"
    "#define __INT_FAST16_WIDTH__ " TOSTRING(__INT_FAST16_WIDTH__) "\n"
    "#define __INT_FAST32_WIDTH__ " TOSTRING(__INT_FAST32_WIDTH__) "\n"
    "#define __INT_FAST64_WIDTH__ " TOSTRING(__INT_FAST64_WIDTH__) "\n"
    "#define __INTPTR_WIDTH__ " TOSTRING(__INTPTR_WIDTH__) "\n"
    "#define __INTMAX_WIDTH__ " TOSTRING(__INTMAX_WIDTH__) "\n"
    "#define __SIZEOF_INT__ " TOSTRING(__SIZEOF_INT__) "\n"
    "#define __SIZEOF_LONG__ " TOSTRING(__SIZEOF_LONG__) "\n"
    "#define __SIZEOF_LONG_LONG__ " TOSTRING(__SIZEOF_LONG_LONG__) "\n"
    "#define __SIZEOF_SHORT__ " TOSTRING(__SIZEOF_SHORT__) "\n"
    "#define __SIZEOF_POINTER__ " TOSTRING(__SIZEOF_POINTER__) "\n"
    "#define __SIZEOF_FLOAT__ " TOSTRING(__SIZEOF_FLOAT__) "\n"
    "#define __SIZEOF_DOUBLE__ " TOSTRING(__SIZEOF_DOUBLE__) "\n"
    "#define __SIZEOF_LONG_DOUBLE__ " TOSTRING(__SIZEOF_LONG_DOUBLE__) "\n"
    "#define __SIZEOF_SIZE_T__ " TOSTRING(__SIZEOF_SIZE_T__) "\n"
    "#define __SIZEOF_WCHAR_T__ " TOSTRING(__SIZEOF_WCHAR_T__) "\n"
    "#define __SIZEOF_WINT_T__ " TOSTRING(__SIZEOF_WINT_T__) "\n"
    "#define __SIZEOF_PTRDIFF_T__ " TOSTRING(__SIZEOF_PTRDIFF_T__) "\n"
#endif
    "\n";



const char* TARGET_X86_MSVC_PREDEFINED_MACROS =

#ifdef __EMSCRIPTEN__
//include dir on emscripten
"#pragma dir \"c:/\"\n"
#endif

CAKE_STANDARD_MACROS
"#define _WIN32 1\n"
"#define _INTEGRAL_MAX_BITS 64\n"
"#define _MSC_VER 1944\n"
"#define _M_IX86 600\n"
"#define __pragma(a)\n"
"\n";

const char* TARGET_X64_MSVC_PREDEFINED_MACROS =

#ifdef __EMSCRIPTEN__
//include dir on emscripten
"#pragma dir \"c:/\"\n"
#endif

CAKE_STANDARD_MACROS
"#define _WIN32 1\n"
"#define _WIN64 1\n"
"#define _INTEGRAL_MAX_BITS 64\n"
"#define _MSC_VER 1944\n"
"#define _M_IX86 _M_IX86\n"
"#define __pragma(a)\n"
"\n";
