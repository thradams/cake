#pragma once

#include <assert.h>

/*
* Compiler options shared with compiler and preprocessor
*/

/*
  
  *** ADDING NEW TARGET** 
  Increment this number, and add a new enum.
  Then compile. static assert will show places where we need to update

*/

#define NUMBER_OF_TARGETS 6

enum target
{
    TARGET_X86_X64_GCC,
    TARGET_X86_MSVC,
    TARGET_X64_MSVC,    
    TARGET_CCU8,
    TARGET_LCCU16,
    TARGET_CATALINA,
};


extern const char* TARGET_X86_X64_GCC_PREDEFINED_MACROS;
extern const char* TARGET_X86_MSVC_PREDEFINED_MACROS;
extern const char* TARGET_X64_MSVC_PREDEFINED_MACROS;
extern const char* TARGET_CCU8_PREDEFINED_MACROS;
extern const char* TARGET_LCCU16_PREDEFINED_MACROS;
extern const char* TARGET_CATALINA_PREDEFINED_MACROS;
static_assert(NUMBER_OF_TARGETS == 6, "add your new target here - different targets may have different predefined macros");


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

#if defined(__APPLE__) && defined(__MACH__)
#define CAKE_COMPILE_TIME_SELECTED_TARGET TARGET_X86_X64_GCC
#endif


const char* target_intN_suffix(enum target target, int size);
const char* target_uintN_suffix(enum target target, int size);

int parse_target(const char* targetstr, enum target* target);
void print_target_options();

const char* target_to_string(enum target target);
unsigned int target_get_wchar_max(enum target target);

long long target_get_signed_long_max(enum target target);
unsigned long long target_get_unsigned_long_max(enum target target);

long long target_get_signed_int_max(enum target target);
unsigned long long target_get_unsigned_int_max(enum target target);


long long target_get_signed_long_long_max(enum target target);
unsigned long long target_get_unsigned_long_long_max(enum target target);


