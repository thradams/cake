#pragma once

// Argument values for exit()
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

// Maximum value that can be returned by the rand function:
#define RAND_MAX 0x7fff

#ifndef NULL
#define NULL ((void *)0)
#endif

//size_t            ldiv_t            __STDC_VERSION_STDLIB
//                                                    EXIT__FAILURE
//                                                           H__          MB_CUR_MAX
//wchar_t           lldiv_t                           EXIT_SUCCESS
//div_t                               NULL            RAND_MAX

typedef struct _div_t
{
    int quot;
    int rem;
} div_t;

typedef struct _ldiv_t
{
    long quot;
    long rem;
} ldiv_t;

typedef struct _lldiv_t
{
    long long quot;
    long long rem;
} lldiv_t;

 double atof(const char *nptr);
 int atoi(const char *nptr);
 long int atol(const char *nptr);
 long long int atoll(const char *nptr);
 int strfromd(char *restrict s, size_t n, const char *restrict format, double fp);
 int strfromf(char *restrict s, size_t n, const char *restrict format, float fp);
 int strfroml(char *restrict s, size_t n, const char *restrict format, long double fp);

 double strtod(const char *restrict nptr, char **restrict endptr);
 float strtof(const char *restrict nptr, char **restrict endptr);
 long double strtold(const char *restrict nptr, char **restrict endptr);
 long int strtol(const char *restrict nptr, char **restrict endptr, int base);
 long long int strtoll(const char *restrict nptr, char **restrict endptr, int base);
 unsigned long int strtoul(const char *restrict nptr, char **restrict endptr, int base);
 unsigned long long int strtoull(const char *restrict nptr, char **restrict endptr, int  base);
 int rand(void);
 void srand(unsigned int seed);
 void *aligned_alloc(size_t alignment, size_t size);
 void *calloc(size_t nmemb, size_t size);
 void free(void *ptr);
 void free_sized(void *ptr, size_t size);
 void free_aligned_sized(void *ptr, size_t alignment, size_t size);
 void *malloc(size_t size);
 void *realloc(void *ptr, size_t size);
 [[noreturn]] void abort(void);
 int atexit(void (*func)(void));
 int at_quick_exit(void (*func)(void));
 [[noreturn]] void exit(int status);
 [[noreturn]] void _Exit(int status);
 char *getenv(const char *name);
 [[noreturn]] void quick_exit(int status);
 int system(const char *string);
 QVoid *bsearch(const void *key, QVoid *base, size_t nmemb, size_t size,
     int (*compar)(const void *, const void *));
 
 void qsort(void *base, size_t nmemb, size_t size,
       int (*compar)(const void *, const void *));

 int abs(int j);
 long int labs(long int j);
 long long int llabs(long long int j);
 div_t div(int numer, int denom);
 ldiv_t ldiv(long int numer, long int denom);
 lldiv_t lldiv(long long int numer, long long int denom);
 int mblen(const char *s, size_t n);
 int mbtowc(wchar_t * restrict pwc, const char * restrict s, size_t n);
 int wctomb(char *s, wchar_t wc);
 size_t mbstowcs(wchar_t * restrict pwcs, const char * restrict s, size_t n);
 size_t wcstombs(char * restrict s, const wchar_t * restrict pwcs, size_t n);
 size_t memalignment(const void * p);

