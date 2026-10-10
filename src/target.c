//#pragma safety enable

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

/*
  GCC on Linux.
  arm64 (e.g. Raspberry Pi): LP64, plain char is unsigned, 128-bit long double.
  arm32 EABI hard-float (e.g. Raspberry Pi 1/2 with a 32-bit OS): ILP32, plain char is unsigned, long double is double.
*/
static void platform_gcc(_Out struct platform* p, const char* arch)
{
    *p = (struct platform){ 0 };

    p->compiler = "GCC";
    p->os = "LINUX";
    p->code_thread_local_spelling = "__thread";
    p->code_alignas_spelling_fmt = "__attribute__((aligned(%d)))";
    p->predefined_macros = gcc_macros;
    p->builtins = gcc_builtins;
    p->code_alloca_spelling = "__builtin_alloca";
    p->gcc_like_asm_label = true;

    p->bool_n_bits = 8;
    p->bool_type = TYPE_UNSIGNED_CHAR;
    p->bool_alignment = 1;

    p->char_n_bits = 8;
    p->char_alignment = 1;

    p->int8_type = TYPE_SIGNED_CHAR;
    p->int16_type = TYPE_SIGNED_SHORT;
    p->int32_type = TYPE_SIGNED_INT;

    p->short_n_bits = 16;
    p->short_alignment = 2;
    p->int_n_bits = 32;
    p->int_alignment = 4;

    p->long_long_n_bits = 64;
    p->long_long_alignment = 8;
    p->float_n_bits = 32;
    p->float_alignment = 4;

    p->double_n_bits = 64;
    p->double_alignment = 8;

    if (strcmp(arch, "x64") == 0)
    {
        p->name = "x86_64-linux-gnu-gcc";
        p->arch = "X64";
        p->size_t_type = TYPE_UNSIGNED_LONG;
        p->ptrdiff_type = TYPE_SIGNED_LONG;
        p->char_t_type = TYPE_SIGNED_CHAR;
        p->int64_type = TYPE_SIGNED_LONG;
        p->pointer_n_bits = 64;
        p->pointer_alignment = 8;
        p->wchar_t_type = TYPE_SIGNED_INT;
        p->long_n_bits = 64;
        p->long_alignment = 8;
        p->long_double_n_bits = 128;
        p->long_double_alignment = 16;
    }
    else if (strcmp(arch, "arm64") == 0)
    {
        p->name = "aarch64-linux-gnu-gcc";
        p->arch = "ARM64";
        p->size_t_type = TYPE_UNSIGNED_LONG;
        p->ptrdiff_type = TYPE_SIGNED_LONG;
        p->char_t_type = TYPE_UNSIGNED_CHAR;
        p->int64_type = TYPE_SIGNED_LONG;
        p->pointer_n_bits = 64;
        p->pointer_alignment = 8;
        p->wchar_t_type = TYPE_UNSIGNED_INT;
        p->long_n_bits = 64;
        p->long_alignment = 8;
        p->long_double_n_bits = 128;
        p->long_double_alignment = 16;
    }
    else /* arm32 */
    {
        p->name = "arm-linux-gnueabihf-gcc";
        p->arch = "ARM32";
        p->size_t_type = TYPE_UNSIGNED_INT;
        p->ptrdiff_type = TYPE_SIGNED_INT;
        p->char_t_type = TYPE_UNSIGNED_CHAR;
        p->int64_type = TYPE_SIGNED_LONG_LONG;
        p->pointer_n_bits = 32;
        p->pointer_alignment = 4;
        p->wchar_t_type = TYPE_UNSIGNED_INT;
        p->long_n_bits = 32;
        p->long_alignment = 4;
        p->long_double_n_bits = 64;
        p->long_double_alignment = 8;
    }
}

static void platform_clang_macos_arm64(_Out struct platform* p)
{
    *p = (struct platform){ 0 };

    p->name = "aarch64-apple-darwin-clang";
    p->compiler = "CLANG";
    p->os = "MACOS";
    p->arch = "ARM64";
    p->code_thread_local_spelling = "__thread";
    p->code_alignas_spelling_fmt = "__attribute__((aligned(%d)))";
    p->predefined_macros = clang_macros;
    p->builtins = gcc_builtins;
    p->code_alloca_spelling = "__builtin_alloca";

    p->size_t_type = TYPE_UNSIGNED_LONG;
    p->ptrdiff_type = TYPE_SIGNED_LONG;

    p->bool_n_bits = 8;
    p->bool_type = TYPE_UNSIGNED_CHAR;
    p->bool_alignment = 1;

    p->char_n_bits = 8;
    p->char_t_type = TYPE_SIGNED_CHAR;
    p->char_alignment = 1;

    p->int8_type = TYPE_SIGNED_CHAR;
    p->int16_type = TYPE_SIGNED_SHORT;
    p->int32_type = TYPE_SIGNED_INT;
    p->int64_type = TYPE_SIGNED_LONG_LONG;

    p->pointer_n_bits = 64;
    p->pointer_alignment = 8;

    p->wchar_t_type = TYPE_SIGNED_INT;

    p->short_n_bits = 16;
    p->short_alignment = 2;
    p->int_n_bits = 32;
    p->int_alignment = 4;

    p->long_n_bits = 64;
    p->long_alignment = 8;

    p->long_long_n_bits = 64;
    p->long_long_alignment = 8;
    p->float_n_bits = 32;
    p->float_alignment = 4;

    p->double_n_bits = 64;
    p->double_alignment = 8;

    p->long_double_n_bits = 64;
    p->long_double_alignment = 8;
}

static void platform_msvc(_Out struct platform* p, const char* arch)
{
    *p = (struct platform){ 0 };

    p->compiler = "MSVC";
    p->os = "WINDOWS";
    p->code_thread_local_spelling = "__declspec(thread)";
    p->code_alignas_spelling_fmt = "__declspec(align(%d))";
    p->predefined_macros = msvc_macros;
    p->builtins = "";
    p->code_alloca_spelling = "_alloca";
    p->msvc_like_bitfield_layout = true;
    p->msvc_like_object_size_limit = true;
    p->msvc_like_decimal_literal_type = true;
    p->msvc_like_keywords = true;
    p->msvc_like_asm_statement = true;
    p->code_msvc_like_atomics = true;
    p->code_msvc_like_no_member_packed = true;

    p->bool_n_bits = 8;
    p->bool_type = TYPE_UNSIGNED_CHAR;
    p->bool_alignment = 1;

    p->char_n_bits = 8;
    p->char_t_type = TYPE_SIGNED_CHAR;
    p->char_alignment = 1;

    p->int8_type = TYPE_SIGNED_CHAR;
    p->int16_type = TYPE_SIGNED_SHORT;
    p->int32_type = TYPE_SIGNED_INT;
    p->int64_type = TYPE_SIGNED_LONG_LONG;

    p->wchar_t_type = TYPE_UNSIGNED_SHORT;

    p->short_n_bits = 16;
    p->short_alignment = 2;
    p->int_n_bits = 32;
    p->int_alignment = 4;

    p->long_n_bits = 32;
    p->long_alignment = 4;

    p->long_long_n_bits = 64;
    p->long_long_alignment = 8;
    p->float_n_bits = 32;
    p->float_alignment = 4;

    p->double_n_bits = 64;
    p->double_alignment = 8;

    p->long_double_n_bits = 64;
    p->long_double_alignment = 8;

    if (strcmp(arch, "x64") == 0)
    {
        p->name = "x86_64-pc-windows-msvc";
        p->arch = "X64";
        p->size_t_type = TYPE_UNSIGNED_LONG_LONG;
        p->ptrdiff_type = TYPE_SIGNED_LONG_LONG;
        p->pointer_n_bits = 64;
        p->pointer_alignment = 8;
    }
    else /* x86 */
    {
        p->name = "i686-pc-windows-msvc";
        p->arch = "X86";
        p->size_t_type = TYPE_UNSIGNED_INT;
        p->ptrdiff_type = TYPE_SIGNED_INT;
        p->pointer_n_bits = 32;
        p->pointer_alignment = 4;
    }
}

/*
  Tiny C Compiler.
  win x64: the LLP64 sizes of x86_64-pc-windows-msvc, gcc syntax.
  linux x64: the LP64 sizes of x86_64-linux-gnu-gcc.
  macos arm64: the sizes of aarch64-apple-darwin-clang.
*/
static void platform_tcc(_Out struct platform* p, const char* os)
{
    *p = (struct platform){ 0 };

    p->compiler = "TCC";
    p->code_thread_local_spelling = "__thread";
    p->code_alignas_spelling_fmt = "__attribute__((aligned(%d)))";
    p->predefined_macros = tcc_macros;
    p->builtins = tcc_builtins;
    p->code_alloca_spelling = "alloca";
    p->gcc_like_asm_label = true;
    p->code_tcc_like_atomics = true;
    p->code_tcc_like_alloca_declaration = true;

    p->bool_n_bits = 8;
    p->bool_type = TYPE_UNSIGNED_CHAR;
    p->bool_alignment = 1;

    p->char_n_bits = 8;
    p->char_t_type = TYPE_SIGNED_CHAR;
    p->char_alignment = 1;

    p->int8_type = TYPE_SIGNED_CHAR;
    p->int16_type = TYPE_SIGNED_SHORT;
    p->int32_type = TYPE_SIGNED_INT;

    p->pointer_n_bits = 64;
    p->pointer_alignment = 8;

    p->short_n_bits = 16;
    p->short_alignment = 2;
    p->int_n_bits = 32;
    p->int_alignment = 4;

    p->long_long_n_bits = 64;
    p->long_long_alignment = 8;
    p->float_n_bits = 32;
    p->float_alignment = 4;

    p->double_n_bits = 64;
    p->double_alignment = 8;

    if (strcmp(os, "win") == 0)
    {
        p->name = "x86_64-w64-mingw32-tcc";
        p->os = "WINDOWS";
        p->arch = "X64";
        p->tcc_like_static_redeclaration = true;
        p->size_t_type = TYPE_UNSIGNED_LONG_LONG;
        p->ptrdiff_type = TYPE_SIGNED_LONG_LONG;
        p->int64_type = TYPE_SIGNED_LONG_LONG;
        p->wchar_t_type = TYPE_UNSIGNED_SHORT;
        p->long_n_bits = 32;
        p->long_alignment = 4;
        p->long_double_n_bits = 64;
        p->long_double_alignment = 8;
    }
    else if (strcmp(os, "linux") == 0)
    {
        p->name = "x86_64-linux-gnu-tcc";
        p->os = "LINUX";
        p->arch = "X64";
        p->size_t_type = TYPE_UNSIGNED_LONG;
        p->ptrdiff_type = TYPE_SIGNED_LONG;
        p->int64_type = TYPE_SIGNED_LONG;
        p->wchar_t_type = TYPE_SIGNED_INT;
        p->long_n_bits = 64;
        p->long_alignment = 8;
        p->long_double_n_bits = 128;
        p->long_double_alignment = 16;
    }
    else /* macos */
    {
        p->name = "aarch64-apple-darwin-tcc";
        p->os = "MACOS";
        p->arch = "ARM64";
        p->size_t_type = TYPE_UNSIGNED_LONG;
        p->ptrdiff_type = TYPE_SIGNED_LONG;
        p->int64_type = TYPE_SIGNED_LONG_LONG;
        p->wchar_t_type = TYPE_SIGNED_INT;
        p->long_n_bits = 64;
        p->long_alignment = 8;
        p->long_double_n_bits = 64;
        p->long_double_alignment = 8;
        p->code_tcc_like_no_builtin_inf = true;
    }
}

static void platform_catalina(_Out struct platform* p)
{
    *p = (struct platform){ 0 };

    p->name = "propeller-catalina";
    p->code_thread_local_spelling = "/*thread*/";
    p->code_alignas_spelling_fmt = "__attribute__((aligned(%d)))";
    p->predefined_macros = catalina_macros;
    p->builtins = catalina_builtins;
    p->code_alloca_spelling = "__builtin_alloca";

    p->size_t_type = TYPE_UNSIGNED_INT;
    p->ptrdiff_type = TYPE_SIGNED_INT;

    p->bool_n_bits = 8;
    p->bool_type = TYPE_UNSIGNED_CHAR;
    p->bool_alignment = 1;

    p->char_n_bits = 8;
    p->char_t_type = TYPE_UNSIGNED_CHAR;
    p->char_alignment = 1;

    p->int8_type = TYPE_SIGNED_CHAR;
    p->int16_type = TYPE_SIGNED_SHORT;
    p->int32_type = TYPE_SIGNED_INT;
    p->int64_type = TYPE_SIGNED_LONG_LONG;

    p->pointer_n_bits = 32;
    p->pointer_alignment = 4;

    p->wchar_t_type = TYPE_UNSIGNED_SHORT;
    p->short_n_bits = 16;
    p->short_alignment = 2;
    p->int_n_bits = 32;
    p->int_alignment = 4;

    p->long_n_bits = 32;
    p->long_alignment = 4;

    p->long_long_n_bits = 32;
    p->long_long_alignment = 4;

    p->float_n_bits = 32;
    p->float_alignment = 4;

    p->double_n_bits = 32;
    p->double_alignment = 4;

    p->long_double_n_bits = 32;
    p->long_double_alignment = 4;
}

/* fills *p with the platform cake itself was built for */
void platform_default(_Out struct platform* p)
{
#if defined(_WIN32) && defined(_WIN64) && defined(__TINYC__)
    platform_tcc(p, "win");
#elif defined(__linux__) && defined(__x86_64__) && defined(__TINYC__)
    platform_tcc(p, "linux");
#elif defined(__APPLE__) && (defined(__aarch64__) || defined(__arm64__)) && defined(__TINYC__)
    platform_tcc(p, "macos");
#elif defined(_WIN32) && defined(_WIN64)
    platform_msvc(p, "x64");
#elif defined(_WIN32) && !defined(_WIN64)
    platform_msvc(p, "x86");
#elif !defined(_WIN32) && (defined(__x86_64__) || defined(_M_X64) || defined(__EMSCRIPTEN__))
    platform_gcc(p, "x64");
#elif defined(__APPLE__) && (defined(__aarch64__) || defined(__arm64__))
    platform_clang_macos_arm64(p);
#elif defined(__linux__) && defined(__aarch64__)
    platform_gcc(p, "arm64");
#elif defined(__linux__) && defined(__arm__)
    platform_gcc(p, "arm32");
#elif defined(__CATALINA__)
    platform_catalina(p);
#else
#error "unknown host platform"
#endif
}

int parse_target(const char* targetstr, struct platform* p_platform)
{
    if (strcmp(targetstr, "default") == 0)
        platform_default(p_platform);
    else if (strcmp(targetstr, "x86_64-linux-gnu-gcc") == 0)
        platform_gcc(p_platform, "x64");
    else if (strcmp(targetstr, "i686-pc-windows-msvc") == 0)
        platform_msvc(p_platform, "x86");
    else if (strcmp(targetstr, "x86_64-pc-windows-msvc") == 0)
        platform_msvc(p_platform, "x64");
    else if (strcmp(targetstr, "propeller-catalina") == 0)
        platform_catalina(p_platform);
    else if (strcmp(targetstr, "aarch64-apple-darwin-clang") == 0)
        platform_clang_macos_arm64(p_platform);
    else if (strcmp(targetstr, "x86_64-w64-mingw32-tcc") == 0)
        platform_tcc(p_platform, "win");
    else if (strcmp(targetstr, "x86_64-linux-gnu-tcc") == 0)
        platform_tcc(p_platform, "linux");
    else if (strcmp(targetstr, "aarch64-apple-darwin-tcc") == 0)
        platform_tcc(p_platform, "macos");
    else if (strcmp(targetstr, "aarch64-linux-gnu-gcc") == 0)
        platform_gcc(p_platform, "arm64");
    else if (strcmp(targetstr, "arm-linux-gnueabihf-gcc") == 0)
        platform_gcc(p_platform, "arm32");
    else
        return 1; //error

    return 0;
}

void print_target_options()
{
    printf("default x86_64-linux-gnu-gcc i686-pc-windows-msvc x86_64-pc-windows-msvc propeller-catalina aarch64-apple-darwin-clang x86_64-w64-mingw32-tcc x86_64-linux-gnu-tcc aarch64-apple-darwin-tcc aarch64-linux-gnu-gcc arm-linux-gnueabihf-gcc\n");
}

static bool str_is(const char* _Opt a, const char* b)
{
    return a != NULL && strcmp(a, b) == 0;
}

bool platform_os_is(const struct platform* p_platform, const char* os)
{
    return str_is(p_platform->os, os);
}

bool platform_arch_is(const struct platform* p_platform, const char* arch)
{
    return str_is(p_platform->arch, arch);
}

long long target_signed_max(const struct platform* target, enum object_type type)
{
    const int bits = target_get_num_of_bits(target, type);
    _Assert(bits <= sizeof(long long) * CHAR_BIT);

    if (bits >= sizeof(long long) * CHAR_BIT)
    {
        return LLONG_MAX;
    }

    return (1LL << (bits - 1)) - 1; // 2^(bits-1) - 1    
}

long long target_signed_min(const struct platform* target, enum object_type type)
{
    const int bits = target_get_num_of_bits(target, type);
    _Assert(bits <= sizeof(long long) * CHAR_BIT);

    if (bits >= sizeof(long long) * CHAR_BIT)
    {
        return LLONG_MIN;
    }

    return -(1LL << (bits - 1));
}

unsigned long long target_unsigned_max(const struct platform* target, enum object_type type)
{
    const int bits = target_get_num_of_bits(target, type);
    _Assert(bits <= sizeof(unsigned long long) * CHAR_BIT);

    if (bits >= sizeof(unsigned long long) * CHAR_BIT)
        return ULLONG_MAX;

    return (1ULL << bits) - 1;
}

int target_get_num_of_bits(const struct platform* target, enum object_type type)
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
        return target->char_n_bits;

    case TYPE_SIGNED_SHORT:
    case TYPE_UNSIGNED_SHORT:
        return target->short_n_bits;

    case TYPE_SIGNED_INT:
    case TYPE_UNSIGNED_INT:
        return target->int_n_bits;

    case TYPE_SIGNED_LONG:
    case TYPE_UNSIGNED_LONG:
        return target->long_n_bits;

    case TYPE_SIGNED_LONG_LONG:
    case TYPE_UNSIGNED_LONG_LONG:
        return target->long_long_n_bits;

    case TYPE_FLOAT:
        return target->float_n_bits;

    case TYPE_DOUBLE:
        return target->double_n_bits;

    case TYPE_LONG_DOUBLE:
        return target->long_double_n_bits;
    
    default:
        break;
    }

    _Assert(false);
    return 0;
}

#ifdef TEST
#include "unit_test.h"

void target_self_test()
{
    struct platform host;
    platform_default(&host);

    assert(target_unsigned_max(&host, TYPE_UNSIGNED_CHAR) == UCHAR_MAX);
    assert(target_unsigned_max(&host, TYPE_UNSIGNED_SHORT) == USHRT_MAX);
    assert(target_unsigned_max(&host, TYPE_UNSIGNED_INT) == UINT_MAX);
    assert(target_unsigned_max(&host, TYPE_UNSIGNED_LONG) == ULONG_MAX);
    assert(target_unsigned_max(&host, TYPE_UNSIGNED_LONG_LONG) == ULLONG_MAX);

    assert(target_signed_max(&host, TYPE_SIGNED_CHAR) == SCHAR_MAX);
    assert(target_signed_max(&host, TYPE_SIGNED_SHORT) == SHRT_MAX);
    assert(target_signed_max(&host, TYPE_SIGNED_INT) == INT_MAX);
    assert(target_signed_max(&host, TYPE_SIGNED_LONG) == LONG_MAX);
    assert(target_signed_max(&host, TYPE_SIGNED_LONG_LONG) == LLONG_MAX);

    assert(target_get_num_of_bits(&host, TYPE_SIGNED_CHAR) == sizeof(char) * CHAR_BIT);
    assert(target_get_num_of_bits(&host, TYPE_SIGNED_SHORT) == sizeof(short) * CHAR_BIT);
    assert(target_get_num_of_bits(&host, TYPE_SIGNED_INT) == sizeof(int) * CHAR_BIT);
    assert(target_get_num_of_bits(&host, TYPE_SIGNED_LONG) == sizeof(long) * CHAR_BIT);
    assert(target_get_num_of_bits(&host, TYPE_SIGNED_LONG_LONG) == sizeof(long long) * CHAR_BIT);

    assert(target_get_num_of_bits(&host, TYPE_LONG_DOUBLE) == sizeof(long double) * CHAR_BIT);

    assert(target_get_num_of_bits(&host, host.size_t_type) == sizeof(sizeof(1)) * CHAR_BIT);

    assert(target_get_num_of_bits(&host, host.wchar_t_type) == sizeof(L' ') * CHAR_BIT);

#if CHAR_MIN < 0
    assert(host.char_t_type == TYPE_SIGNED_CHAR);
#else
    assert(host.char_t_type == TYPE_UNSIGNED_CHAR);
#endif

}
#endif