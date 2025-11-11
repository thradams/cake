#pragma once


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
    TYPE_LONG_DOUBLE
};


enum target
{
    TARGET_X86_X64_GCC,
    TARGET_X86_MSVC,
    TARGET_X64_MSVC,    
    TARGET_CCU8,
    TARGET_LCCU16,
    TARGET_CATALINA,

};
#define NUMBER_OF_TARGETS  6

struct platform
{
    const char* name;
    
    const char* thread_local_attr;
    const char * alignas_fmt_must_have_one_percent_d;

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


int parse_target(const char* targetstr, enum target* target);
void print_target_options();
struct platform* get_platform(enum  target target);
int target_get_num_of_bits(enum target target, enum object_type type);
int parse_target(const char* targetstr, enum target* target);
void print_target_options();
const char* target_get_predefined_macros(enum target e);
const char* target_get_builtins(enum target e);


long long target_signed_max(enum  target target, enum object_type type);
unsigned long long target_unsigned_max(enum  target target, enum object_type type);


#if defined(_WIN32) && defined(_WIN64)
#define CAKE_COMPILE_TIME_SELECTED_TARGET TARGET_X64_MSVC
#endif

#if defined(_WIN32) && !defined(_WIN64)
#define CAKE_COMPILE_TIME_SELECTED_TARGET TARGET_X86_MSVC
#endif

#if !defined(_WIN32) && (defined(__x86_64__) || defined(_M_X64))
#define CAKE_COMPILE_TIME_SELECTED_TARGET TARGET_X86_X64_GCC
#endif

#ifdef __EMSCRIPTEN__
#define CAKE_COMPILE_TIME_SELECTED_TARGET TARGET_X86_X64_GCC
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define CAKE_COMPILE_TIME_SELECTED_TARGET TARGET_X86_X64_GCC
#endif
