#pragma once
#include <stdbool.h>


enum object_type
{
    TYPE_SIGNED_CHAR,
    TYPE_UNSIGNED_CHAR,

    TYPE_SIGNED_SHORT,
    TYPE_UNSIGNED_SHORT,

    TYPE_SIGNED_INT,
    TYPE_UNSIGNED_INT,

    TYPE_SIGNED_LONG,
    TYPE_UNSIGNED_LONG,

    TYPE_SIGNED_LONG_LONG,
    TYPE_UNSIGNED_LONG_LONG,

    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_LONG_DOUBLE,

    /* unsigned bitfield = TYPE_UNSIGNED_BITFIELD_1 (13) ... TYPE_UNSIGNED_BITFIELD_128 (140)*/
    
    TYPE_UNSIGNED_BITFIELD_1,
    TYPE_UNSIGNED_BITFIELD_128 = (TYPE_UNSIGNED_BITFIELD_1 + 127),

    /* signed bitfield = TYPE_SIGNED_BITFIELD_1(141) ... TYPE_SIGNED_BITFIELD_128 (268)*/    
    TYPE_SIGNED_BITFIELD_1,
    TYPE_SIGNED_BITFIELD_128 = (TYPE_SIGNED_BITFIELD_1 + 127),

    /*
      C23 bit-precise integers, unsigned _BitInt(N) / _BitInt(N).
      Same value representation as bitfields (N-bit value held in a host
      long long) but they are not subjected to integer promotion and have
      their own conversion rank, so they need a range of their own.
    */
    TYPE_UNSIGNED_BITINT_1,
    TYPE_UNSIGNED_BITINT_128 = (TYPE_UNSIGNED_BITINT_1 + 127),

    TYPE_SIGNED_BITINT_1,
    TYPE_SIGNED_BITINT_128 = (TYPE_SIGNED_BITINT_1 + 127),
};


struct platform
{
    const char* _Opt name;   /* a zeroed struct options has none until fill_options */

    /* __CAKE_TARGET_COMPILER_<compiler>, __CAKE_TARGET_OS_<os>, __CAKE_TARGET_ARCH_<arch>;
       NULL defines nothing */
    const char* _Opt compiler;
    const char* _Opt os;
    const char* _Opt arch;

    const char* _Opt predefined_macros;
    const char* _Opt builtins;

    /* code gen flags; false is the GCC behavior */
    const char* _Opt code_thread_local_spelling;
    const char* _Opt code_alignas_spelling_fmt;  /* must have one %d */
    const char* _Opt code_alloca_spelling;
    bool code_msvc_like_atomics;               /* atomics use _Interlocked* */
    bool code_tcc_like_atomics;                /* atomics use tcc's lock; neither = __atomic_* */
    bool code_msvc_like_no_member_packed;      /* no __attribute__((packed)) on a member */
    bool code_tcc_like_no_builtin_inf;         /* no __builtin_inf / __builtin_fabs */
    bool code_tcc_like_alloca_declaration;     /* alloca needs a prototype */

    /*
      Behaviors of a compiler that differ from C/GCC; false is the C/GCC behavior.
      Named by the compiler that has it, but any target can imitate it.
    */
    bool msvc_like_bitfield_layout;       /* bit-field units and unnamed bit-field alignment as MSVC */
    bool msvc_like_object_size_limit;     /* one object cannot exceed 0x7FFFFFFF bytes */
    bool msvc_like_decimal_literal_type;  /* unsuffixed decimal literal can be unsigned long (C90) */
    bool msvc_like_keywords;              /* __ptr32, __ptr64 ... */
    bool msvc_like_asm_statement;         /* __asm { ... } instead of asm("...") */
    bool gcc_like_asm_label;              /* int x __asm("name"); */
    bool tcc_like_static_redeclaration;   /* block scope static after a non-static declaration */

    int bool_n_bits;
    int bool_alignment;
    enum object_type bool_type;

    int char_n_bits;
    enum object_type char_t_type;
    int char_alignment;

    int short_n_bits;
    int short_alignment;

    int int_n_bits;
    int int_alignment;

    int long_n_bits;
    int long_alignment;

    int long_long_n_bits;
    int long_long_alignment;

    int float_n_bits;
    int float_alignment;

    int double_n_bits;
    int double_alignment;

    int long_double_n_bits;
    int long_double_alignment;

    int pointer_n_bits;
    int pointer_alignment;

    /*typedefs*/
    enum object_type wchar_t_type;
    enum object_type int8_type;
    enum object_type int16_type;
    enum object_type int32_type;
    enum object_type int64_type;

    enum object_type size_t_type;
    enum object_type ptrdiff_type;
};


/* fills *p_platform with the platform named targetstr ("default" is the host) */
int parse_target(const char* targetstr, struct platform* p_platform);
void print_target_options();
void platform_default(_Out struct platform* p);

bool platform_os_is(const struct platform* p_platform, const char* os);
bool platform_arch_is(const struct platform* p_platform, const char* arch);

int target_get_num_of_bits(const struct platform* target, enum object_type type);
long long target_signed_max(const struct platform* target, enum object_type type);
long long target_signed_min(const struct platform* target, enum object_type type);
unsigned long long target_unsigned_max(const struct platform* target, enum object_type type);
