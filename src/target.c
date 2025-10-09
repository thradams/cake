#include "target.h"
#include <limits.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
const char* target_name[NUMBER_OF_TARGETS] = {
     "x86_x64_gcc",
     "x86_msvc",
     "x64_msvc",
     "ccu8",
     "lcc-u16",
     "catalina"
};



int parse_target(const char* targetstr, enum target* target)
{
    for (int i = 0; i < NUMBER_OF_TARGETS; i++)
    {
        if (strcmp(targetstr + 8, target_name[i]) == 0)
        {
            *target = i;
            return 0;
        }
    }

    return 1; //error
}

void print_target_options()
{
    for (int i = 0; i < NUMBER_OF_TARGETS; i++)
    {
        printf("%s ", target_name[i]);
    }

    printf("\n");
}

const char* target_intN_suffix(enum target target, int size)
{
    /*
      The number we place in the output needs
      to be the same size as the compiler understand it.
    */

    switch (target)
    {
    case TARGET_X86_X64_GCC:
        if (size == 64) return "";
        break;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        if (size == 64) return "LL";
        break;
    case TARGET_LCCU16:
    case TARGET_CCU8:
        if (size == 32) return "L";
        if (size == 64) return "LL";
        break;

    case TARGET_CATALINA:
        if (size == 64) return "LL";
        break;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
    return "";
}

const char* target_uintN_suffix(enum target target, int size)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:
        if (size == 32) return "U";
        if (size == 64) return "U";
        break;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        if (size == 32) return "U";
        if (size == 64) return "ULL";
        break;
    case TARGET_LCCU16:
    case TARGET_CCU8:
        if (size == 32) return "UL";
        if (size == 64) return "ULL";
        break;
    case TARGET_CATALINA:
        if (size == 64) return "ULL";
        break;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
    return "";
}

const char* target_to_string(enum target target)
{
    if (target >= 0 && target < NUMBER_OF_TARGETS)
    {
        return target_name[(int)target];
    }
    assert(false);
    return "";
}

unsigned int target_get_wchar_max(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:
        return 2147483647;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        return 0xffff;
    
    case TARGET_LCCU16:
    case TARGET_CCU8:
        return 255;
    case TARGET_CATALINA:
        return 255;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
    assert(false);
    return 0;
}

long long target_get_signed_long_max(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:
        return 0x7fffffffffffffffLL;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        return 2147483647LL;
    
    case TARGET_LCCU16:
    case TARGET_CCU8:
        return 2147483647LL;

    case TARGET_CATALINA:
        return 2147483647LL;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
    assert(false);
    return 0;
}

long long target_get_signed_long_long_max(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:
        return 9223372036854775807LL;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        return 9223372036854775807LL;
    
    case TARGET_LCCU16:
    case TARGET_CCU8:
        return 2147483647LL;

    case TARGET_CATALINA:
        return 2147483647LL;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
    assert(false);
    return 0;
}

unsigned long long target_get_unsigned_long_long_max(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:
        return 18446744073709551615ULL;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        return 18446744073709551615ULL;
    
    case TARGET_LCCU16:
    case TARGET_CCU8:
        return 4294967295ULL;

    case TARGET_CATALINA:
        return 4294967295ULL;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
    assert(false);
    return 0;
}
unsigned long long target_get_unsigned_long_max(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:
        return 18446744073709551615ULL;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        return 0xffffffff;
    
    case TARGET_LCCU16:
    case TARGET_CCU8:
        return 4294967295ULL;

    case TARGET_CATALINA:
        return 0xffffffff;

    }
    static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
    assert(false);
    return 0;
}

long long target_get_signed_int_max(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:
        return 2147483647LL;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        return 2147483647LL;

    case TARGET_LCCU16:
    case TARGET_CCU8:
        return 32767ULL;

    case TARGET_CATALINA:
        return 2147483647LL;
    }
    static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
    assert(false);
    return 0;
}

unsigned long long target_get_unsigned_int_max(enum target target)
{
    switch (target)
    {
    case TARGET_X86_X64_GCC:
        return 4294967295ULL;

    case TARGET_X86_MSVC:
    case TARGET_X64_MSVC:
        return 4294967295ULL;

    case TARGET_LCCU16:
    case TARGET_CCU8:
        return 65535ULL;

    case TARGET_CATALINA:
        return 4294967295ULL;
    }

    static_assert(NUMBER_OF_TARGETS == 6, "add new target here");
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



const char* TARGET_X86_X64_GCC_PREDEFINED_MACROS =
#ifdef __EMSCRIPTEN__
//include dir on emscripten
"#pragma dir \"c:/\"\n"
#endif

CAKE_STANDARD_MACROS
"#define __linux__\n"
"#define __GNUC__  16\n"
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
"#define _M_X64 100\n"
"#define __pragma(a)\n"
"\n";


const char* TARGET_CCU8_PREDEFINED_MACROS =

#ifdef __EMSCRIPTEN__
//include dir on emscripten
"#pragma dir \"c:/\"\n"
#endif

CAKE_STANDARD_MACROS
"#define __EI()\n"
"#define __DI()\n"
"#define __asm()\n"
"#define __SEGBASE_N()\n"
"#define __SEGBASE_F()\n"
"#define __SEGSIZE()\n"
"#define __near\n"
"#define __far\n"
"#define __huge\n"
"#define __PACKED\n"
"#define __UNPACKED\n"
"#define __noreg\n"
"#define __STDC__\n"
"#define __CCU8__\n"
"#define __VERSION__\n"
"#define __ARCHITECTURE__\n"
"#define __DEBUG__\n"
"#define __MS__\n"
"#define __ML__\n"
"#define __ML__\n"
"#define __UNSIGNEDCHAR__\n"
"#define __NOFAR__\n"
"#define __LCCU16__\n"
"#define __LAPISOMF__\n"
"\n";

const char* TARGET_LCCU16_PREDEFINED_MACROS =

#ifdef __EMSCRIPTEN__
//include dir on emscripten
"#pragma dir \"c:/\"\n"
#endif

CAKE_STANDARD_MACROS
"#define __EI()\n"
"#define __DI()\n"
"#define __asm()\n"
"#define __SEGBASE_N()\n"
"#define __SEGBASE_F()\n"
"#define __SEGSIZE()\n"
"#define __near\n"
"#define __far\n"
"#define __huge\n"
"#define __PACKED\n"
"#define __UNPACKED\n"
"#define __noreg\n"
"#define __STDC__\n"
"#define __CCU8__\n"
"#define __VERSION__\n"
"#define __ARCHITECTURE__\n"
"#define __DEBUG__\n"
"#define __MS__\n"
"#define __ML__\n"
"#define __ML__\n"
"#define __UNSIGNEDCHAR__\n"
"#define __NOFAR__\n"
"#define __LCCU16__\n"
"#define __LAPISOMF__\n"
"\n";

const char* TARGET_CATALINA_PREDEFINED_MACROS =

#ifdef __EMSCRIPTEN__
//include dir on emscripten
"#pragma dir \"c:/\"\n"
#endif

CAKE_STANDARD_MACROS
"#define ___CATALINA__\n"
"#define ___CATALYST__\n"
"\n";


static_assert(NUMBER_OF_TARGETS == 6, "add new target here");


