
#pragma once
#include "annotations.h"

typedef long long fpos_t;
typedef int size_t;
typedef int wchar_t;

[[nodiscard]] double atof(const char* nptr);
[[nodiscard]] int atoi(const char* nptr);
[[nodiscard]] long int atol(const char* nptr);
[[nodiscard]] long long int atoll(const char* nptr);

double strtod(const char* restrict nptr, char** restrict endptr);
float strtof(const char* restrict nptr, char** restrict endptr);
long double strtold(const char* restrict nptr, char** restrict endptr);
long int strtol(const char* restrict nptr, char** restrict endptr, int base);
long long int strtoll(const char* restrict nptr, char** restrict endptr, int base);
unsigned long int strtoul(const char* restrict nptr, char** restrict endptr, int base);
unsigned long long int strtoull(const char* restrict nptr, char** restrict endptr, int base);
int rand(void);
void srand(unsigned int seed);
void* aligned_alloc(size_t alignment, size_t size);
[[nodiscard]] void* calloc(size_t nmemb, size_t size);
void free(void* ptr);

[[nodiscard]] void* malloc(size_t size);
[[nodiscard]] void* realloc(void* ptr, size_t size);

[[noreturn]] void abort(void);
int atexit(void (*func)(void));
int at_quick_exit(void (*func)(void));
[[noreturn]] void exit(int status);
[[noreturn]] void _Exit(int status);
char* getenv(const char* name);
[[noreturn]] void quick_exit(int status);
int system(const char* string);

#ifndef NULL
#define NULL ((void*)0)
#endif


void* malloc(size_t i) extern {
    _add_attr(return, MUST_FREE);
}

void* calloc(size_t nmemb, size_t size) extern {
    _add_attr(return, MUST_FREE);
}

void free(void* p) extern {
    static_assert(_has_attr(p, MUST_FREE));
    _del_attr(p, MUST_FREE);
}

void* moveptr(void* p) extern {
    static_assert(_has_attr(p, MUST_FREE));
    _del_attr(p, MUST_FREE);
    _add_attr(p, UNINITIALIZED);
    _add_attr(return, MUST_FREE);
}
