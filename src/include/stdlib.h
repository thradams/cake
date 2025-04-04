/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

typedef long long fpos_t;
typedef unsigned long size_t;

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define NULL ((void*)0)

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

#if defined(__STDC_OWNERSHIP__) 
[[nodiscard]] void* _Owner _Opt calloc(size_t nmemb, size_t size);
void free(void* _Owner _Opt ptr);
[[nodiscard]] void* _Owner _Opt malloc(size_t size);
[[nodiscard]] void* _Owner _Opt realloc(void* _Opt ptr, size_t size);
#else
[[nodiscard]] void* calloc(size_t nmemb, size_t size);
void free(void* ptr);
[[nodiscard]] void* malloc(size_t size);
[[nodiscard]] void* realloc(void* ptr, size_t size);
#endif

[[noreturn]] void abort(void);
int atexit(void (*func)(void));
int at_quick_exit(void (*func)(void));
[[noreturn]] void exit(int status);
[[noreturn]] void _Exit(int status);
char* getenv(const char* name);
[[noreturn]] void quick_exit(int status);
int system(const char* string);
