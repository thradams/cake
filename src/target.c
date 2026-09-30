#pragma safety enable

#include "cake_compat.h"
#include "target.h"
#include <limits.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

static char gcc_builtins[] =
{
 #include "include/builtins/gcc_builtins.h.include"
 , 0
};

static char catalina_builtins[] =
{
 #include "include/builtins/catalina_builtins.h.include"
 , 0
};

static char tcc_builtins[] =
{
 #include "include/builtins/tcc_builtins.h.include"
 , 0
};

static char gcc_macros[] =
{
 #include "include/builtins/gcc_macros.h.include"
 , 0
};

static char clang_macros[] =
{
 #include "include/builtins/clang_macros.h.include"
 , 0
};

static char msvc_macros[] =
{
 #include "include/builtins/msvc_macros.h.include"
 , 0
};

static char tcc_macros[] =
{
 #include "include/builtins/tcc_macros.h.include"
 , 0
};

static char catalina_macros[] =
{
 #include "include/builtins/catalina_macros.h.include"
 , 0
};

static char ccu8_macros[] =
{
 #include "include/builtins/ccu8_macros.h.include"
 , 0
};


#ifndef _Countof
#define _Countof(A) (sizeof(A)/sizeof((A)[0]))
#endif


static struct platform platform_x86_x64_gcc =
{
  .name = "gcc-linux-x64",
  .compiler = "GCC",
  .os = "LINUX",
  .arch = "X64",
  .thread_local_attr = "__thread",
  .alignas_fmt_must_have_one_percent_d = "__attribute__((aligned(%d)))",

  .size_t_type = TYPE_UNSIGNED_LONG,
  .ptrdiff_type = TYPE_SIGNED_LONG, //long

  .bool_n_bits = 8,
  .bool_type = TYPE_UNSIGNED_CHAR,
  .bool_alignment = 1,

  .char_n_bits = 8,
  .char_t_type = TYPE_SIGNED_CHAR,
  .char_alignment = 1,



  .int8_type = TYPE_SIGNED_CHAR,
  .int16_type = TYPE_SIGNED_SHORT,
  .int32_type = TYPE_SIGNED_INT,
  .int64_type = TYPE_SIGNED_LONG,

  .pointer_n_bits = 64,
  .pointer_alignment = 8,


  .wchar_t_type = TYPE_SIGNED_INT,

  .short_n_bits = 16,
  .short_alignment = 2,
  .int_n_bits = 32,
  .int_alignment = 4,

  .long_n_bits = 64,
  .long_alignment = 8,

  .long_long_n_bits = 64,
  .long_long_alignment = 8,
  .float_n_bits = 32,
  .float_alignment = 4,

  .double_n_bits = 64,
  .double_alignment = 8,

  .long_double_n_bits = 128,
  .long_double_alignment = 16,

};

static struct platform platform_macos_arm64 =
{
  .name = "clang-macos-arm64",
  .compiler = "CLANG",
  .os = "MACOS",
  .arch = "ARM64",
  .thread_local_attr = "__thread",
  .alignas_fmt_must_have_one_percent_d = "__attribute__((aligned(%d)))",

  .size_t_type = TYPE_UNSIGNED_LONG,
  .ptrdiff_type = TYPE_SIGNED_LONG, //long

  .bool_n_bits = 8,
  .bool_type = TYPE_UNSIGNED_CHAR,
  .bool_alignment = 1,

  .char_n_bits = 8,
  .char_t_type = TYPE_SIGNED_CHAR,
  .char_alignment = 1,



  .int8_type = TYPE_SIGNED_CHAR,
  .int16_type = TYPE_SIGNED_SHORT,
  .int32_type = TYPE_SIGNED_INT,
  .int64_type = TYPE_SIGNED_LONG_LONG,

  .pointer_n_bits = 64,
  .pointer_alignment = 8,


  .wchar_t_type = TYPE_SIGNED_INT,

  .short_n_bits = 16,
  .short_alignment = 2,
  .int_n_bits = 32,
  .int_alignment = 4,

  .long_n_bits = 64,
  .long_alignment = 8,

  .long_long_n_bits = 64,
  .long_long_alignment = 8,
  .float_n_bits = 32,
  .float_alignment = 4,

  .double_n_bits = 64,
  .double_alignment = 8,

  .long_double_n_bits = 64,
  .long_double_alignment = 8,

};

static struct platform platform_x86_msvc =
{
  .name = "msvc-win-x86",
  .compiler = "MSVC",
  .os = "WINDOWS",
  .arch = "X86",
  .thread_local_attr = "__declspec(thread)",
  .alignas_fmt_must_have_one_percent_d = "__declspec(align(%d))",

  .size_t_type = TYPE_UNSIGNED_INT,
  .ptrdiff_type = TYPE_SIGNED_INT, //long

  .bool_n_bits = 8,
  .bool_type = TYPE_UNSIGNED_CHAR,
  .bool_alignment = 1,

  .char_n_bits = 8,
  .char_t_type = TYPE_SIGNED_CHAR,
  .char_alignment = 1,


  .int8_type = TYPE_SIGNED_CHAR,
  .int16_type = TYPE_SIGNED_SHORT,
  .int32_type = TYPE_SIGNED_INT,
  .int64_type = TYPE_SIGNED_LONG_LONG,

  .pointer_n_bits = 32,
  .pointer_alignment = 4,


  .wchar_t_type = TYPE_UNSIGNED_SHORT,

  .short_n_bits = 16,
  .short_alignment = 2,
  .int_n_bits = 32,
  .int_alignment = 4,

  .long_n_bits = 32,
  .long_alignment = 4,

  .long_long_n_bits = 64,
  .long_long_alignment = 8,
  .float_n_bits = 32,
  .float_alignment = 4,

  .double_n_bits = 64,
  .double_alignment = 8,

  .long_double_n_bits = 64,
  .long_double_alignment = 8,
};

static struct platform platform_x64_msvc =
{
   .name = "msvc-win-x64",
   .compiler = "MSVC",
   .os = "WINDOWS",
   .arch = "X64",
   .thread_local_attr = "__declspec(thread)",
   .alignas_fmt_must_have_one_percent_d = "__declspec(align(%d))",

  .size_t_type = TYPE_UNSIGNED_LONG_LONG,
  .ptrdiff_type = TYPE_SIGNED_LONG_LONG, //long

  .bool_n_bits = 8,
  .bool_type = TYPE_UNSIGNED_CHAR,
  .bool_alignment = 1,

  .char_n_bits = 8,
  .char_t_type = TYPE_SIGNED_CHAR,
  .char_alignment = 1,


  .int8_type = TYPE_SIGNED_CHAR,
  .int16_type = TYPE_SIGNED_SHORT,
  .int32_type = TYPE_SIGNED_INT,
  .int64_type = TYPE_SIGNED_LONG_LONG,

  .pointer_n_bits = 64,
  .pointer_alignment = 8,


  .wchar_t_type = TYPE_UNSIGNED_SHORT,

  .short_n_bits = 16,
  .short_alignment = 2,
  .int_n_bits = 32,
  .int_alignment = 4,

  .long_n_bits = 32,
  .long_alignment = 4,

  .long_long_n_bits = 64,
  .long_long_alignment = 8,
  .float_n_bits = 32,
  .float_alignment = 4,

  .double_n_bits = 64,
  .double_alignment = 8,

  .long_double_n_bits = 64,
  .long_double_alignment = 8,
};

/* Tiny C Compiler on Windows x64: the LLP64 sizes of msvc-win-x64, gcc syntax. */
static struct platform platform_tcc_win_x64 =
{
  .name = "tcc-win-x64",
  .compiler = "TCC",
  .os = "WINDOWS",
  .arch = "X64",
  .thread_local_attr = "__thread",
  .alignas_fmt_must_have_one_percent_d = "__attribute__((aligned(%d)))",

  .size_t_type = TYPE_UNSIGNED_LONG_LONG,
  .ptrdiff_type = TYPE_SIGNED_LONG_LONG,

  .bool_n_bits = 8,
  .bool_type = TYPE_UNSIGNED_CHAR,
  .bool_alignment = 1,

  .char_n_bits = 8,
  .char_t_type = TYPE_SIGNED_CHAR,
  .char_alignment = 1,

  .int8_type = TYPE_SIGNED_CHAR,
  .int16_type = TYPE_SIGNED_SHORT,
  .int32_type = TYPE_SIGNED_INT,
  .int64_type = TYPE_SIGNED_LONG_LONG,

  .pointer_n_bits = 64,
  .pointer_alignment = 8,

  .wchar_t_type = TYPE_UNSIGNED_SHORT,

  .short_n_bits = 16,
  .short_alignment = 2,
  .int_n_bits = 32,
  .int_alignment = 4,

  .long_n_bits = 32,
  .long_alignment = 4,

  .long_long_n_bits = 64,
  .long_long_alignment = 8,
  .float_n_bits = 32,
  .float_alignment = 4,

  .double_n_bits = 64,
  .double_alignment = 8,

  .long_double_n_bits = 64,
  .long_double_alignment = 8,
};

/* Tiny C Compiler on Linux x64: the LP64 sizes of gcc-linux-x64. */
static struct platform platform_tcc_linux_x64 =
{
  .name = "tcc-linux-x64",
  .compiler = "TCC",
  .os = "LINUX",
  .arch = "X64",
  .thread_local_attr = "__thread",
  .alignas_fmt_must_have_one_percent_d = "__attribute__((aligned(%d)))",

  .size_t_type = TYPE_UNSIGNED_LONG,
  .ptrdiff_type = TYPE_SIGNED_LONG, //long

  .bool_n_bits = 8,
  .bool_type = TYPE_UNSIGNED_CHAR,
  .bool_alignment = 1,

  .char_n_bits = 8,
  .char_t_type = TYPE_SIGNED_CHAR,
  .char_alignment = 1,



  .int8_type = TYPE_SIGNED_CHAR,
  .int16_type = TYPE_SIGNED_SHORT,
  .int32_type = TYPE_SIGNED_INT,
  .int64_type = TYPE_SIGNED_LONG,

  .pointer_n_bits = 64,
  .pointer_alignment = 8,


  .wchar_t_type = TYPE_SIGNED_INT,

  .short_n_bits = 16,
  .short_alignment = 2,
  .int_n_bits = 32,
  .int_alignment = 4,

  .long_n_bits = 64,
  .long_alignment = 8,

  .long_long_n_bits = 64,
  .long_long_alignment = 8,
  .float_n_bits = 32,
  .float_alignment = 4,

  .double_n_bits = 64,
  .double_alignment = 8,

  .long_double_n_bits = 128,
  .long_double_alignment = 16,

};

/* Tiny C Compiler on macOS arm64: the sizes of clang-macos-arm64. */
static struct platform platform_tcc_macos_arm64 =
{
  .name = "tcc-macos-arm64",
  .compiler = "TCC",
  .os = "MACOS",
  .arch = "ARM64",
  .thread_local_attr = "__thread",
  .alignas_fmt_must_have_one_percent_d = "__attribute__((aligned(%d)))",

  .size_t_type = TYPE_UNSIGNED_LONG,
  .ptrdiff_type = TYPE_SIGNED_LONG, //long

  .bool_n_bits = 8,
  .bool_type = TYPE_UNSIGNED_CHAR,
  .bool_alignment = 1,

  .char_n_bits = 8,
  .char_t_type = TYPE_SIGNED_CHAR,
  .char_alignment = 1,



  .int8_type = TYPE_SIGNED_CHAR,
  .int16_type = TYPE_SIGNED_SHORT,
  .int32_type = TYPE_SIGNED_INT,
  .int64_type = TYPE_SIGNED_LONG_LONG,

  .pointer_n_bits = 64,
  .pointer_alignment = 8,


  .wchar_t_type = TYPE_SIGNED_INT,

  .short_n_bits = 16,
  .short_alignment = 2,
  .int_n_bits = 32,
  .int_alignment = 4,

  .long_n_bits = 64,
  .long_alignment = 8,

  .long_long_n_bits = 64,
  .long_long_alignment = 8,
  .float_n_bits = 32,
  .float_alignment = 4,

  .double_n_bits = 64,
  .double_alignment = 8,

  .long_double_n_bits = 64,
  .long_double_alignment = 8,

};

/* GCC on Linux aarch64 (e.g. Raspberry Pi): LP64, plain char is unsigned, 128-bit long double. */
static struct platform platform_gcc_linux_arm64 =
{
  .name = "gcc-linux-arm64",
  .compiler = "GCC",
  .os = "LINUX",
  .arch = "ARM64",
  .thread_local_attr = "__thread",
  .alignas_fmt_must_have_one_percent_d = "__attribute__((aligned(%d)))",

  .size_t_type = TYPE_UNSIGNED_LONG,
  .ptrdiff_type = TYPE_SIGNED_LONG, //long

  .bool_n_bits = 8,
  .bool_type = TYPE_UNSIGNED_CHAR,
  .bool_alignment = 1,

  .char_n_bits = 8,
  .char_t_type = TYPE_UNSIGNED_CHAR,
  .char_alignment = 1,



  .int8_type = TYPE_SIGNED_CHAR,
  .int16_type = TYPE_SIGNED_SHORT,
  .int32_type = TYPE_SIGNED_INT,
  .int64_type = TYPE_SIGNED_LONG,

  .pointer_n_bits = 64,
  .pointer_alignment = 8,


  .wchar_t_type = TYPE_UNSIGNED_INT,

  .short_n_bits = 16,
  .short_alignment = 2,
  .int_n_bits = 32,
  .int_alignment = 4,

  .long_n_bits = 64,
  .long_alignment = 8,

  .long_long_n_bits = 64,
  .long_long_alignment = 8,
  .float_n_bits = 32,
  .float_alignment = 4,

  .double_n_bits = 64,
  .double_alignment = 8,

  .long_double_n_bits = 128,
  .long_double_alignment = 16,

};

static struct platform platform_ccu8 =
{
  .name = "ccu8",
  .thread_local_attr = "/*thread*/",
  .alignas_fmt_must_have_one_percent_d = "__attribute__((aligned(%d)))",

  .size_t_type = TYPE_UNSIGNED_INT,
  .ptrdiff_type = TYPE_SIGNED_INT, //long

  .bool_n_bits = 8,
  .bool_type = TYPE_UNSIGNED_CHAR,
  .bool_alignment = 1,

  .char_n_bits = 8,
  .char_t_type = TYPE_SIGNED_CHAR,
  .char_alignment = 1,


  .int8_type = TYPE_SIGNED_CHAR,
  .int16_type = TYPE_SIGNED_SHORT,
  .int32_type = TYPE_SIGNED_INT,
  .int64_type = TYPE_SIGNED_LONG_LONG,

  .pointer_n_bits = 32,
  .pointer_alignment = 8,


  .wchar_t_type = TYPE_UNSIGNED_SHORT,
  .short_n_bits = 16,
  .short_alignment = 2,
  .int_n_bits = 16,
  .int_alignment = 2,

  .long_n_bits = 64,
  .long_alignment = 4,

  .long_long_n_bits = 64,
  .long_long_alignment = 8,
  .float_n_bits = 32,
  .float_alignment = 32,

  .double_n_bits = 64,
  .double_alignment = 8,

  .long_double_n_bits = 64,
  .long_double_alignment = 8,
};

static struct platform platform_catalina =
{
  .name = "catalina",
  .thread_local_attr = "/*thread*/",
  .alignas_fmt_must_have_one_percent_d = "__attribute__((aligned(%d)))",


  .size_t_type = TYPE_UNSIGNED_INT,
  .ptrdiff_type = TYPE_SIGNED_INT, //long

  .bool_n_bits = 8,
  .bool_type = TYPE_UNSIGNED_CHAR,
  .bool_alignment = 1,

  .char_n_bits = 8,
  .char_t_type = TYPE_UNSIGNED_CHAR,
  .char_alignment = 1,


  .int8_type = TYPE_SIGNED_CHAR,
  .int16_type = TYPE_SIGNED_SHORT,
  .int32_type = TYPE_SIGNED_INT,
  .int64_type = TYPE_SIGNED_LONG_LONG,

  .pointer_n_bits = 32,
  .pointer_alignment = 4,


  .wchar_t_type = TYPE_UNSIGNED_SHORT,
  .short_n_bits = 16,
  .short_alignment = 2,
  .int_n_bits = 32,
  .int_alignment = 4,

  .long_n_bits = 32,
  .long_alignment = 4,

  .long_long_n_bits = 32,
  .long_long_alignment = 4,

  .float_n_bits = 32,
  .float_alignment = 4,

  .double_n_bits = 32,
  .double_alignment = 4,

  .long_double_n_bits = 32,
  .long_double_alignment = 4,
};

/* GCC on Linux 32-bit ARM, EABI hard-float (e.g. Raspberry Pi 1/2 with a 32-bit OS): ILP32, plain char is unsigned, long double is double. */
static struct platform platform_gcc_linux_arm32 =
{
  .name = "gcc-linux-arm32",
  .compiler = "GCC",
  .os = "LINUX",
  .arch = "ARM32",
  .thread_local_attr = "__thread",
  .alignas_fmt_must_have_one_percent_d = "__attribute__((aligned(%d)))",

  .size_t_type = TYPE_UNSIGNED_INT,
  .ptrdiff_type = TYPE_SIGNED_INT,

  .bool_n_bits = 8,
  .bool_type = TYPE_UNSIGNED_CHAR,
  .bool_alignment = 1,

  .char_n_bits = 8,
  .char_t_type = TYPE_UNSIGNED_CHAR,
  .char_alignment = 1,

  .int8_type = TYPE_SIGNED_CHAR,
  .int16_type = TYPE_SIGNED_SHORT,
  .int32_type = TYPE_SIGNED_INT,
  .int64_type = TYPE_SIGNED_LONG_LONG,

  .pointer_n_bits = 32,
  .pointer_alignment = 4,

  .wchar_t_type = TYPE_UNSIGNED_INT,

  .short_n_bits = 16,
  .short_alignment = 2,
  .int_n_bits = 32,
  .int_alignment = 4,

  .long_n_bits = 32,
  .long_alignment = 4,

  .long_long_n_bits = 64,
  .long_long_alignment = 8,
  .float_n_bits = 32,
  .float_alignment = 4,

  .double_n_bits = 64,
  .double_alignment = 8,

  .long_double_n_bits = 64,
  .long_double_alignment = 8,
};

static struct platform* platforms[NUMBER_OF_TARGETS] =
{
        [TARGET_GCC_LINUX_X64] = &platform_x86_x64_gcc,
        [TARGET_MSVC_WIN_X86] = &platform_x86_msvc,
        [TARGET_MSVC_WIN_X64] = &platform_x64_msvc,
        [TARGET_CCU8] = &platform_ccu8,
        [TARGET_LCCU16] = &platform_ccu8,
        [TARGET_CATALINA] = &platform_catalina,
        [TARGET_CLANG_MACOS_ARM64] = &platform_macos_arm64,
        [TARGET_TCC_WIN_X64] = &platform_tcc_win_x64,
        [TARGET_TCC_LINUX_X64] = &platform_tcc_linux_x64,
        [TARGET_TCC_MACOS_ARM64] = &platform_tcc_macos_arm64,
        [TARGET_GCC_LINUX_ARM64] = &platform_gcc_linux_arm64,
        [TARGET_GCC_LINUX_ARM32] = &platform_gcc_linux_arm32
};

static_assert(NUMBER_OF_TARGETS == 12, "insert platform here");

int parse_target(const char* targetstr, enum target* target)
{
    if (strcmp(targetstr, "default") == 0)
    {
        *target = TARGET_DEFAULT;
        return 0;
    }

    for (int i = 0; i < _Countof(platforms); i++)
    {
        if (strcmp(targetstr, platforms[i]->name) == 0)
        {
            *target = i;
            return 0;
        }
    }

    return 1; //error
}

void print_target_options()
{
    printf("default ");
    for (int i = 0; i < _Countof(platforms); i++)
    {
        printf("%s ", platforms[i]->name);
    }

    printf("\n");
}

struct platform* get_platform(enum  target target)
{
    return platforms[target];
}

long long target_signed_max(enum target target, enum object_type type)
{
    const int bits = target_get_num_of_bits(target, type);
    _Assert(bits <= sizeof(long long) * CHAR_BIT);

    if (bits >= sizeof(long long) * CHAR_BIT)
    {
        return LLONG_MAX;
    }

    return (1LL << (bits - 1)) - 1; // 2^(bits-1) - 1    
}

long long target_signed_min(enum target target, enum object_type type)
{
    const int bits = target_get_num_of_bits(target, type);
    _Assert(bits <= sizeof(long long) * CHAR_BIT);

    if (bits >= sizeof(long long) * CHAR_BIT)
    {
        return LLONG_MIN;
    }

    return -(1LL << (bits - 1));
}

unsigned long long target_unsigned_max(enum  target target, enum object_type type)
{
    const int bits = target_get_num_of_bits(target, type);
    _Assert(bits <= sizeof(unsigned long long) * CHAR_BIT);

    if (bits >= sizeof(unsigned long long) * CHAR_BIT)
        return ULLONG_MAX;

    return (1ULL << bits) - 1;
}

int target_get_num_of_bits(enum target target, enum object_type type)
{
    if (type >= TYPE_UNSIGNED_BITFIELD_1 && type <= TYPE_UNSIGNED_BITFIELD_128)
    {
        return (int)(type - TYPE_UNSIGNED_BITFIELD_1 + 1);
    }

    if (type >= TYPE_SIGNED_BITFIELD_1 && type <= TYPE_SIGNED_BITFIELD_128)
    {
        return (int)(type - TYPE_SIGNED_BITFIELD_1 + 1);
    }

    if (type >= TYPE_UNSIGNED_BITINT_1 && type <= TYPE_UNSIGNED_BITINT_128)
    {
        return (int)(type - TYPE_UNSIGNED_BITINT_1 + 1);
    }

    if (type >= TYPE_SIGNED_BITINT_1 && type <= TYPE_SIGNED_BITINT_128)
    {
        return (int)(type - TYPE_SIGNED_BITINT_1 + 1);
    }

    switch (type)
    {
    case TYPE_SIGNED_CHAR:
    case TYPE_UNSIGNED_CHAR:
        return get_platform(target)->char_n_bits;

    case TYPE_SIGNED_SHORT:
    case TYPE_UNSIGNED_SHORT:
        return get_platform(target)->short_n_bits;

    case TYPE_SIGNED_INT:
    case TYPE_UNSIGNED_INT:
        return get_platform(target)->int_n_bits;

    case TYPE_SIGNED_LONG:
    case TYPE_UNSIGNED_LONG:
        return get_platform(target)->long_n_bits;

    case TYPE_SIGNED_LONG_LONG:
    case TYPE_UNSIGNED_LONG_LONG:
        return get_platform(target)->long_long_n_bits;

    case TYPE_FLOAT:
        return get_platform(target)->float_n_bits;

    case TYPE_DOUBLE:
        return get_platform(target)->double_n_bits;

    case TYPE_LONG_DOUBLE:
        return get_platform(target)->long_double_n_bits;
    
    default:
        break;
    }

    _Assert(false);
    return 0;
}

const char* target_get_predefined_macros(enum target e)
{
    switch (e)
    {
    case TARGET_GCC_LINUX_X64: return gcc_macros;
    case TARGET_MSVC_WIN_X86:    return msvc_macros;
    case TARGET_MSVC_WIN_X64:    return msvc_macros;
    case TARGET_CCU8:        return ccu8_macros;
    case TARGET_LCCU16:      return ccu8_macros;
    case TARGET_CATALINA:    return catalina_macros;
    case TARGET_CLANG_MACOS_ARM64: return clang_macros;
    case TARGET_TCC_WIN_X64:     return tcc_macros;
    case TARGET_TCC_LINUX_X64:   return tcc_macros;
    case TARGET_TCC_MACOS_ARM64: return tcc_macros;
    case TARGET_GCC_LINUX_ARM64: return gcc_macros;
    case TARGET_GCC_LINUX_ARM32: return gcc_macros;
    }
    return "";
};


const char* target_get_alloca(enum target e)
{
    switch (e)
    {
    case TARGET_GCC_LINUX_X64: return "__builtin_alloca";
    case TARGET_MSVC_WIN_X86:    return "_alloca";
    case TARGET_MSVC_WIN_X64:    return "_alloca";
    case TARGET_CCU8:        return "__builtin_alloca";
    case TARGET_LCCU16:      return "__builtin_alloca";
    case TARGET_CATALINA:    return "__builtin_alloca";
    case TARGET_CLANG_MACOS_ARM64: return "__builtin_alloca";
    case TARGET_TCC_WIN_X64:     return "alloca";
    case TARGET_TCC_LINUX_X64:   return "alloca";
    case TARGET_TCC_MACOS_ARM64: return "alloca";
    case TARGET_GCC_LINUX_ARM64: return "__builtin_alloca";
    case TARGET_GCC_LINUX_ARM32: return "__builtin_alloca";
    }
    return "";
}
const char* target_get_builtins(enum target e)
{
    switch (e)
    {
    case TARGET_GCC_LINUX_X64: return gcc_builtins;
    case TARGET_MSVC_WIN_X86:    return "";
    case TARGET_MSVC_WIN_X64:    return "";
    case TARGET_CCU8:        return "";
    case TARGET_LCCU16:      return "";
    case TARGET_CATALINA:    return catalina_builtins;
    case TARGET_CLANG_MACOS_ARM64: return gcc_builtins;
    case TARGET_TCC_WIN_X64:     return tcc_builtins;
    case TARGET_TCC_LINUX_X64:   return tcc_builtins;
    case TARGET_TCC_MACOS_ARM64: return tcc_builtins;
    case TARGET_GCC_LINUX_ARM64: return gcc_builtins;
    case TARGET_GCC_LINUX_ARM32: return gcc_builtins;
    }
    return "";
}

#ifdef TEST
#include "unit_test.h"

void target_self_test()
{
    assert(target_unsigned_max(TARGET_DEFAULT, TYPE_UNSIGNED_CHAR) == UCHAR_MAX);
    assert(target_unsigned_max(TARGET_DEFAULT, TYPE_UNSIGNED_SHORT) == USHRT_MAX);
    assert(target_unsigned_max(TARGET_DEFAULT, TYPE_UNSIGNED_INT) == UINT_MAX);
    assert(target_unsigned_max(TARGET_DEFAULT, TYPE_UNSIGNED_LONG) == ULONG_MAX);
    assert(target_unsigned_max(TARGET_DEFAULT, TYPE_UNSIGNED_LONG_LONG) == ULLONG_MAX);

    assert(target_signed_max(TARGET_DEFAULT, TYPE_SIGNED_CHAR) == SCHAR_MAX);
    assert(target_signed_max(TARGET_DEFAULT, TYPE_SIGNED_SHORT) == SHRT_MAX);
    assert(target_signed_max(TARGET_DEFAULT, TYPE_SIGNED_INT) == INT_MAX);
    assert(target_signed_max(TARGET_DEFAULT, TYPE_SIGNED_LONG) == LONG_MAX);
    assert(target_signed_max(TARGET_DEFAULT, TYPE_SIGNED_LONG_LONG) == LLONG_MAX);

    assert(target_get_num_of_bits(TARGET_DEFAULT, TYPE_SIGNED_CHAR) == sizeof(char) * CHAR_BIT);
    assert(target_get_num_of_bits(TARGET_DEFAULT, TYPE_SIGNED_SHORT) == sizeof(short) * CHAR_BIT);
    assert(target_get_num_of_bits(TARGET_DEFAULT, TYPE_SIGNED_INT) == sizeof(int) * CHAR_BIT);
    assert(target_get_num_of_bits(TARGET_DEFAULT, TYPE_SIGNED_LONG) == sizeof(long) * CHAR_BIT);
    assert(target_get_num_of_bits(TARGET_DEFAULT, TYPE_SIGNED_LONG_LONG) == sizeof(long long) * CHAR_BIT);

    assert(target_get_num_of_bits(TARGET_DEFAULT, TYPE_LONG_DOUBLE) == sizeof(long double) * CHAR_BIT);


    assert(target_get_num_of_bits(TARGET_DEFAULT, get_platform(TARGET_DEFAULT)->size_t_type) == sizeof(sizeof(1)) * CHAR_BIT);

    assert(target_get_num_of_bits(TARGET_DEFAULT, get_platform(TARGET_DEFAULT)->wchar_t_type) == sizeof(L' ') * CHAR_BIT);


#if CHAR_MIN < 0
    assert(get_platform(TARGET_DEFAULT)->char_t_type == TYPE_SIGNED_CHAR);
#else
    assert(get_platform(TARGET_DEFAULT)->char_t_type == TYPE_UNSIGNED_CHAR);
#endif


}
#endif