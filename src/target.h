#pragma once


/*
* Compiler options shared with compiler and preprocessor
*/

enum target
{
    TARGET_DEFAULT = 0,
    TARGET_X86_X64_GCC,
    TARGET_X86_MSVC,
    TARGET_X64_MSVC,
};

const char* target_to_string(enum target target);
//unsigned long long target_get_unsigned_short_max(enum target target);
int target_get_wchar_max(enum target target);

long long target_get_signed_long_max(enum target target);
unsigned long long target_get_unsigned_long_max(enum target target);
long long target_get_signed_long_long_max(enum target target);

extern const char* TARGET_DEFAULT_PREDEFINED_MACROS;
extern const char* TARGET_X86_X64_GCC_PREDEFINED_MACROS;
extern const char* TARGET_X86_MSVC_PREDEFINED_MACROS;
extern const char* TARGET_X64_MSVC_PREDEFINED_MACROS;
