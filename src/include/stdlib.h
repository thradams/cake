/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#include <__cake_types.h>

#define __STDC_VERSION_STDLIB_H__ 202311L

typedef __cake_size_t size_t;
typedef __cake_wchar_t wchar_t;

typedef struct { int quot; int rem; } div_t;
typedef struct { long quot; long rem; } ldiv_t;
typedef struct { long long quot; long long rem; } lldiv_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#if defined(_WIN32)
#define RAND_MAX 0x7fff
int* ___mb_cur_max_func(void);
#define MB_CUR_MAX (*___mb_cur_max_func())
#elif defined(__APPLE__)
#define RAND_MAX 0x7fffffff
extern int __mb_cur_max;
#define MB_CUR_MAX __mb_cur_max
#elif defined(__linux__)
#define RAND_MAX 2147483647
size_t __ctype_get_mb_cur_max(void);
#define MB_CUR_MAX (__ctype_get_mb_cur_max())
#else
#define RAND_MAX 0x7fff
#define MB_CUR_MAX 1
#endif

/* numeric conversion */
[[nodiscard]] double atof(const char* nptr);
[[nodiscard]] int atoi(const char* nptr);
[[nodiscard]] long int atol(const char* nptr);
[[nodiscard]] long long int atoll(const char* nptr);
double strtod(const char* restrict nptr, char** _Opt restrict endptr);
float strtof(const char* restrict nptr, char** _Opt restrict endptr);
long double strtold(const char* restrict nptr, char** _Opt restrict endptr);
long int strtol(const char* restrict nptr, char** _Opt restrict endptr, int base);
long long int strtoll(const char* restrict nptr, char** _Opt restrict endptr, int base);
unsigned long int strtoul(const char* restrict nptr, char** _Opt restrict endptr, int base);
unsigned long long int strtoull(const char* restrict nptr, char** _Opt restrict endptr, int base);

/* pseudo random */
int rand(void);
void srand(unsigned int seed);

/* memory management */
#if !defined(_WIN32)
[[nodiscard]] void* _Owner _Opt aligned_alloc(size_t alignment, size_t size);
#endif
[[nodiscard]] void* _Owner _Opt _Clear calloc(size_t nmemb, size_t size);
void free(void* _Owner _Opt ptr);
[[nodiscard]] void* _Owner _Opt _Uninitialized malloc(size_t size);
[[nodiscard]] void* _Owner _Opt realloc(void* _Opt ptr, size_t size);

/* communication with the environment */
[[noreturn]] void abort(void);
int atexit(void (*func)(void));
int at_quick_exit(void (*func)(void));
[[noreturn]] void exit(int status);
[[noreturn]] void _Exit(int status);
char* _Opt getenv(const char* name);
[[noreturn]] void quick_exit(int status);
int system(const char* _Opt string);

/* searching and sorting */
void* _Opt bsearch(const void* key, const void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*));
void qsort(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*));

/* integer arithmetic */
[[nodiscard]] int abs(int j);
[[nodiscard]] long int labs(long int j);
[[nodiscard]] long long int llabs(long long int j);
[[nodiscard]] div_t div(int numer, int denom);
[[nodiscard]] ldiv_t ldiv(long int numer, long int denom);
[[nodiscard]] lldiv_t lldiv(long long int numer, long long int denom);

/* multibyte/wide character conversion */
int mblen(const char* _Opt s, size_t n);
int mbtowc(wchar_t* _Opt restrict pwc, const char* _Opt restrict s, size_t n);
int wctomb(char* _Opt s, wchar_t wc);
size_t mbstowcs(wchar_t* restrict pwcs, const char* restrict s, size_t n);
size_t wcstombs(char* restrict s, const wchar_t* restrict pwcs, size_t n);

#if defined(__linux__) || defined(__APPLE__)
/* POSIX */
int setenv(const char* name, const char* value, int overwrite);
int unsetenv(const char* name);
int putenv(char* string);
char* _Opt realpath(const char* restrict path, char* _Opt restrict resolved_path);
#endif

#if defined(_WIN32)
int putenv(char* string);
int _putenv(const char* string);
char* _Opt _fullpath(char* _Opt absPath, const char* relPath, size_t maxLength);
[[nodiscard]] void* _Owner _Opt _aligned_malloc(size_t size, size_t alignment);
void _aligned_free(void* _Owner _Opt ptr);
#endif

#else


    #ifdef _WIN64
        typedef unsigned __int64 size_t;    
    #elif defined _WIN32
        typedef unsigned int     size_t;
    #endif
    
    #ifdef __linux__
        typedef __SIZE_TYPE__ size_t;
    #endif
    
    #ifdef __APPLE__
        typedef __SIZE_TYPE__ size_t;
    #endif
    

    void* _Owner _Opt _Clear calloc(size_t nmemb, size_t size);
    void free(void* _Owner _Opt ptr);
    void* _Owner _Opt _Uninitialized malloc(size_t size);
    void* _Owner _Opt realloc(void* _Opt ptr, size_t size);


    long strtol(char const* _String, char** _Opt _EndPtr, int _Radix);
    long long strtoll(char const* _String, char** _Opt _EndPtr, int _Radix);
    double strtod(char const* _String, char** _Opt _EndPtr);
    long double strtold(char const* _String, char** _Opt _EndPtr);
    unsigned long long strtoull(char const* _String, char** _Opt _EndPtr, int _Radix);
    float strtof(char const* _String, char** _Opt _EndPtr);

    char* _Opt _fullpath(char* _Opt absPath, const char* relPath, size_t maxLength);

    /* ..then lets include the system headers ... */
    #include_next <stdlib.h>

#endif
