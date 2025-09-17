#pragma once


/*
* Compiler options shared with compiler and preprocessor
*/

enum target
{
    TARGET_X86_X64_GCC,
    TARGET_X86_MSVC,
    TARGET_X64_MSVC,
};

#ifdef _WIN32 
#ifdef _WIN64
#define CAKE_COMPILE_TIME_SELECTED_TARGET TARGET_X64_MSVC
#else
#define CAKE_COMPILE_TIME_SELECTED_TARGET TARGET_X86_MSVC
#endif
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define CAKE_COMPILE_TIME_SELECTED_TARGET TARGET_X86_X64_GCC
#endif

#ifdef __EMSCRIPTEN__
#define CAKE_COMPILE_TIME_SELECTED_TARGET TARGET_X86_X64_GCC
#endif


/*these strings are used as -target= option*/
#define  TARGET_X86_X64_GCC_STR "x86_x64_gcc"
#define  TARGET_X86_MSVC_STR "x86_msvc"
#define  TARGET_X64_MSVC_STR "x64_msvc"

const char* target_intN_suffix(enum target target, int size);
const char* target_uintN_suffix(enum target target, int size);

int parse_target(const char* targetstr, enum target* target);
void print_target_options();

const char* target_to_string(enum target target);
unsigned int target_get_wchar_max(enum target target);

long long target_get_signed_long_max(enum target target);
unsigned long long target_get_unsigned_long_max(enum target target);
long long target_get_signed_long_long_max(enum target target);


extern const char* TARGET_X86_X64_GCC_PREDEFINED_MACROS;
extern const char* TARGET_X86_MSVC_PREDEFINED_MACROS;
extern const char* TARGET_X64_MSVC_PREDEFINED_MACROS;
