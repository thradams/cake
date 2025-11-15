#include "target.h"
#include <limits.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

static char gcc_builtins_include[] =
{
 #include "include/x86_x64_gcc_builtins.h.include"
 ,0
};

static char x86_x64_gcc_macros[] =
{
 #include "include/x86_x64_gcc_macros.h.include"
 ,0
};

static char x86_msvc_macros[] =
{
 #include "include/x86_msvc_macros.h.include"
 ,0
};

static char x64_msvc_macros[] =
{
 #include "include/x64_msvc_macros.h.include"
 ,0
};


static char catalina_macros[] =
{
 #include "include/catalina_macros.h.include"
 ,0
};

static char catalina_builtins[] =
{
 #include "include/catalina_builtins.h.include"
 ,0
};

static char ccu8_macros[] =
{
 #include "include/ccu8_macros.h.include"
 ,0
};


#ifndef _Countof
#define _Countof(A) (sizeof(A)/sizeof((A)[0]))
#endif


static struct platform platform_x86_x64_gcc =
{
  .name = "x86_x64_gcc",
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

static struct platform platform_x86_msvc =
{
  .name = "x86_msvc",
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
   .name = "x64_msvc",
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

static struct platform* platforms[NUMBER_OF_TARGETS] =
{
        [TARGET_X86_X64_GCC] = &platform_x86_x64_gcc,
        [TARGET_X86_MSVC] = &platform_x86_msvc,
        [TARGET_X64_MSVC] = &platform_x64_msvc,
        [TARGET_CCU8] = &platform_ccu8,
        [TARGET_LCCU16] = &platform_ccu8,
        [TARGET_CATALINA] = &platform_catalina,
};

static_assert(NUMBER_OF_TARGETS == 6, "insert platform here");

int parse_target(const char* targetstr, enum target* target)
{
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
    assert(bits <= sizeof(long long) * CHAR_BIT);

    if (bits >= sizeof(long long) * CHAR_BIT)
    {
        return LLONG_MAX;
    }

    return (1LL << (bits - 1)) - 1; // 2^(bits-1) - 1    
}

unsigned long long target_unsigned_max(enum  target target, enum object_type type)
{
    const int bits = target_get_num_of_bits(target, type);
    assert(bits <= sizeof(unsigned long long) * CHAR_BIT);

    if (bits >= sizeof(unsigned long long) * CHAR_BIT)
        return ULLONG_MAX;

    return (1ULL << bits) - 1;
}

int target_get_num_of_bits(enum target target, enum object_type type)
{
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
    }

    assert(false);
    return 0;
}

const char* target_get_predefined_macros(enum target e)
{
    switch (e)
    {
    case TARGET_X86_X64_GCC: return x86_x64_gcc_macros;
    case TARGET_X86_MSVC:    return x86_msvc_macros;
    case TARGET_X64_MSVC:    return x64_msvc_macros;
    case TARGET_CCU8:        return ccu8_macros;
    case TARGET_LCCU16:      return ccu8_macros;
    case TARGET_CATALINA:    return catalina_macros;
    }
    return "";
};



const char* target_get_builtins(enum target e)
{
    switch (e)
    {
    case TARGET_X86_X64_GCC: return gcc_builtins_include;
    case TARGET_X86_MSVC:    return "";
    case TARGET_X64_MSVC:    return "";
    case TARGET_CCU8:        return "";
    case TARGET_LCCU16:      return "";
    case TARGET_CATALINA:    return catalina_builtins;
    }
    return "";
}

#ifdef TEST
#include "unit_test.h"

    void target_self_test()
    {
        assert(target_unsigned_max(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_UNSIGNED_CHAR) == UCHAR_MAX);
        assert(target_unsigned_max(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_UNSIGNED_SHORT) == USHRT_MAX);
        assert(target_unsigned_max(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_UNSIGNED_INT) == UINT_MAX);
        assert(target_unsigned_max(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_UNSIGNED_LONG) == ULONG_MAX);
        assert(target_unsigned_max(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_UNSIGNED_LONG_LONG) == ULLONG_MAX);

        assert(target_signed_max(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_SIGNED_CHAR) == CHAR_MAX);
        assert(target_signed_max(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_SIGNED_SHORT) == SHRT_MAX);
        assert(target_signed_max(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_SIGNED_INT) == INT_MAX);
        assert(target_signed_max(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_SIGNED_LONG) == LONG_MAX);
        assert(target_signed_max(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_SIGNED_LONG_LONG) == LLONG_MAX);

        assert(target_get_num_of_bits(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_SIGNED_CHAR) == sizeof(char) * CHAR_BIT);
        assert(target_get_num_of_bits(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_SIGNED_SHORT) == sizeof(short) * CHAR_BIT);
        assert(target_get_num_of_bits(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_SIGNED_INT) == sizeof(int) * CHAR_BIT);
        assert(target_get_num_of_bits(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_SIGNED_LONG) == sizeof(long) * CHAR_BIT);
        assert(target_get_num_of_bits(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_SIGNED_LONG_LONG) == sizeof(long long) * CHAR_BIT);
        
        assert(target_get_num_of_bits(CAKE_COMPILE_TIME_SELECTED_TARGET, TYPE_LONG_DOUBLE) == sizeof(long double) * CHAR_BIT);


        assert(target_get_num_of_bits(CAKE_COMPILE_TIME_SELECTED_TARGET, get_platform(CAKE_COMPILE_TIME_SELECTED_TARGET)->size_t_type) == sizeof(sizeof(1)) * CHAR_BIT);

        assert(target_get_num_of_bits(CAKE_COMPILE_TIME_SELECTED_TARGET, get_platform(CAKE_COMPILE_TIME_SELECTED_TARGET)->wchar_t_type) == sizeof(L' ') * CHAR_BIT);


#if CHAR_MIN < 0
        assert(get_platform(CAKE_COMPILE_TIME_SELECTED_TARGET)->char_t_type == TYPE_SIGNED_CHAR);
#else
        assert(get_platform(CAKE_COMPILE_TIME_SELECTED_TARGET)->char_t_type == TYPE_UNSIGNED_CHAR);
#endif


    }
#endif