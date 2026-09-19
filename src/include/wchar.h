/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#include <__cake_types.h>

#define __STDC_VERSION_WCHAR_H__ 202311L

typedef __cake_size_t size_t;
typedef __cake_wchar_t wchar_t;
typedef __cake_wint_t wint_t;
typedef __cake_va_list va_list;

typedef struct __cake_FILE FILE;

typedef __cake_mbstate_t mbstate_t;

struct tm;

#ifndef NULL
#define NULL ((void*)0)
#endif

#define WEOF ((wint_t)-1)

#if defined(_WIN32) || !defined(__SIZE_TYPE__)
#define WCHAR_MIN 0
#define WCHAR_MAX 0xffff
#else
#define WCHAR_MIN (-0x7fffffff - 1)
#define WCHAR_MAX 0x7fffffff
#endif

/* formatted wide character input/output */
#if defined(_WIN32)

/* like stdio.h: the msvc CRT only exports __stdio_common_v*, the family is
   inline in its headers */
#include <stdarg.h>
#include <stdio.h>

int __stdio_common_vfwprintf(unsigned long long options, FILE* stream, const wchar_t* format, void* _Opt locale, va_list arg);
int __stdio_common_vswprintf(unsigned long long options, wchar_t* _Opt s, size_t n, const wchar_t* format, void* _Opt locale, va_list arg);
int __stdio_common_vfwscanf(unsigned long long options, FILE* stream, const wchar_t* format, void* _Opt locale, va_list arg);
int __stdio_common_vswscanf(unsigned long long options, const wchar_t* s, size_t n, const wchar_t* format, void* _Opt locale, va_list arg);

static inline int vfwprintf(FILE* restrict stream, const wchar_t* restrict format, va_list arg)
{
    return __stdio_common_vfwprintf(0, stream, format, NULL, arg);
}

static inline int vwprintf(const wchar_t* restrict format, va_list arg)
{
    return __stdio_common_vfwprintf(0, stdout, format, NULL, arg);
}

static inline int vswprintf(wchar_t* restrict s, size_t n, const wchar_t* restrict format, va_list arg)
{
    int r = __stdio_common_vswprintf(0, s, n, format, NULL, arg);
    return r < 0 ? -1 : r;
}

static inline int vfwscanf(FILE* restrict stream, const wchar_t* restrict format, va_list arg)
{
    return __stdio_common_vfwscanf(0, stream, format, NULL, arg);
}

static inline int vwscanf(const wchar_t* restrict format, va_list arg)
{
    return __stdio_common_vfwscanf(0, stdin, format, NULL, arg);
}

static inline int vswscanf(const wchar_t* restrict s, const wchar_t* restrict format, va_list arg)
{
    return __stdio_common_vswscanf(0, s, (size_t)-1, format, NULL, arg);
}

static inline int fwprintf(FILE* restrict stream, const wchar_t* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int r = vfwprintf(stream, format, arg);
    va_end(arg);
    return r;
}

static inline int wprintf(const wchar_t* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int r = vwprintf(format, arg);
    va_end(arg);
    return r;
}

static inline int swprintf(wchar_t* restrict s, size_t n, const wchar_t* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int r = vswprintf(s, n, format, arg);
    va_end(arg);
    return r;
}

static inline int fwscanf(FILE* restrict stream, const wchar_t* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int r = vfwscanf(stream, format, arg);
    va_end(arg);
    return r;
}

static inline int wscanf(const wchar_t* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int r = vwscanf(format, arg);
    va_end(arg);
    return r;
}

static inline int swscanf(const wchar_t* restrict s, const wchar_t* restrict format, ...)
{
    va_list arg;
    va_start(arg, format);
    int r = vswscanf(s, format, arg);
    va_end(arg);
    return r;
}

#else

int fwprintf(FILE* restrict stream, const wchar_t* restrict format, ...);
int fwscanf(FILE* restrict stream, const wchar_t* restrict format, ...);
int swprintf(wchar_t* restrict s, size_t n, const wchar_t* restrict format, ...);
int swscanf(const wchar_t* restrict s, const wchar_t* restrict format, ...);
int vfwprintf(FILE* restrict stream, const wchar_t* restrict format, va_list arg);
int vfwscanf(FILE* restrict stream, const wchar_t* restrict format, va_list arg);
int vswprintf(wchar_t* restrict s, size_t n, const wchar_t* restrict format, va_list arg);
int vswscanf(const wchar_t* restrict s, const wchar_t* restrict format, va_list arg);
int vwprintf(const wchar_t* restrict format, va_list arg);
int vwscanf(const wchar_t* restrict format, va_list arg);
int wprintf(const wchar_t* restrict format, ...);
int wscanf(const wchar_t* restrict format, ...);

#endif

/* wide character input/output */
wint_t fgetwc(FILE* stream);
wchar_t* _Opt fgetws(wchar_t* restrict s, int n, FILE* restrict stream);
wint_t fputwc(wchar_t c, FILE* stream);
int fputws(const wchar_t* restrict s, FILE* restrict stream);
#if defined(_WIN32)
/* msvc streams have no orientation, the CRT header just returns mode */
static inline int fwide(FILE* stream, int mode)
{
    (void)stream;
    return mode;
}
#else
int fwide(FILE* stream, int mode);
#endif
wint_t getwc(FILE* stream);
wint_t getwchar(void);
wint_t putwc(wchar_t c, FILE* stream);
wint_t putwchar(wchar_t c);
wint_t ungetwc(wint_t c, FILE* stream);

/* general wide string utilities */
double wcstod(const wchar_t* restrict nptr, wchar_t** _Opt restrict endptr);
float wcstof(const wchar_t* restrict nptr, wchar_t** _Opt restrict endptr);
long double wcstold(const wchar_t* restrict nptr, wchar_t** _Opt restrict endptr);
long int wcstol(const wchar_t* restrict nptr, wchar_t** _Opt restrict endptr, int base);
long long int wcstoll(const wchar_t* restrict nptr, wchar_t** _Opt restrict endptr, int base);
unsigned long int wcstoul(const wchar_t* restrict nptr, wchar_t** _Opt restrict endptr, int base);
unsigned long long int wcstoull(const wchar_t* restrict nptr, wchar_t** _Opt restrict endptr, int base);

wchar_t* wcscpy(wchar_t* restrict s1, const wchar_t* restrict s2);
wchar_t* wcsncpy(wchar_t* restrict s1, const wchar_t* restrict s2, size_t n);
wchar_t* wmemcpy(wchar_t* restrict s1, const wchar_t* restrict s2, size_t n);
wchar_t* wmemmove(wchar_t* s1, const wchar_t* s2, size_t n);
wchar_t* wcscat(wchar_t* restrict s1, const wchar_t* restrict s2);
wchar_t* wcsncat(wchar_t* restrict s1, const wchar_t* restrict s2, size_t n);
int wcscmp(const wchar_t* s1, const wchar_t* s2);
int wcscoll(const wchar_t* s1, const wchar_t* s2);
int wcsncmp(const wchar_t* s1, const wchar_t* s2, size_t n);
size_t wcsxfrm(wchar_t* _Opt restrict s1, const wchar_t* restrict s2, size_t n);
#if defined(_WIN32)
/* inline in the msvc CRT headers */
static inline int wmemcmp(const wchar_t* s1, const wchar_t* s2, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        if (s1[i] != s2[i])
            return s1[i] < s2[i] ? -1 : 1;
    }
    return 0;
}
#else
int wmemcmp(const wchar_t* s1, const wchar_t* s2, size_t n);
#endif
wchar_t* _Opt wcschr(const wchar_t* s, wchar_t c);
size_t wcscspn(const wchar_t* s1, const wchar_t* s2);
wchar_t* _Opt wcspbrk(const wchar_t* s1, const wchar_t* s2);
wchar_t* _Opt wcsrchr(const wchar_t* s, wchar_t c);
size_t wcsspn(const wchar_t* s1, const wchar_t* s2);
wchar_t* _Opt wcsstr(const wchar_t* s1, const wchar_t* s2);
wchar_t* _Opt wcstok(wchar_t* _Opt restrict s1, const wchar_t* restrict s2, wchar_t** restrict ptr);
#if defined(_WIN32)
static inline wchar_t* _Opt wmemchr(const wchar_t* s, wchar_t c, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        if (s[i] == c)
            return (wchar_t*)&s[i];
    }
    return NULL;
}
#else
wchar_t* _Opt wmemchr(const wchar_t* s, wchar_t c, size_t n);
#endif
size_t wcslen(const wchar_t* s);
size_t wcsnlen(const wchar_t* s, size_t maxlen);
wchar_t* wmemset(wchar_t* s, wchar_t c, size_t n);
wchar_t* _Owner _Opt wcsdup(const wchar_t* s);

#if defined(_WIN32)
/* msvc extensions, declared by its string.h/wchar.h */
wchar_t* _Owner _Opt _wcsdup(const wchar_t* s);
int _wcsicmp(const wchar_t* s1, const wchar_t* s2);
int _wcsnicmp(const wchar_t* s1, const wchar_t* s2, size_t n);
wchar_t* _wcslwr(wchar_t* s);
wchar_t* _wcsupr(wchar_t* s);
wchar_t* _wcsrev(wchar_t* s);
#endif

/* wide character time conversion */
size_t wcsftime(wchar_t* restrict s, size_t maxsize, const wchar_t* restrict format, const struct tm* restrict timeptr);

/* extended multibyte/wide character conversion */
wint_t btowc(int c);
int wctob(wint_t c);
#if defined(_WIN32)
static inline int mbsinit(const mbstate_t* _Opt ps)
{
    return ps == NULL || ps->_Wchar == 0;
}
#else
int mbsinit(const mbstate_t* _Opt ps);
#endif
size_t mbrlen(const char* _Opt restrict s, size_t n, mbstate_t* _Opt restrict ps);
size_t mbrtowc(wchar_t* _Opt restrict pwc, const char* _Opt restrict s, size_t n, mbstate_t* _Opt restrict ps);
size_t wcrtomb(char* _Opt restrict s, wchar_t wc, mbstate_t* _Opt restrict ps);
size_t mbsrtowcs(wchar_t* _Opt restrict dst, const char** restrict src, size_t len, mbstate_t* _Opt restrict ps);
size_t wcsrtombs(char* _Opt restrict dst, const wchar_t** restrict src, size_t len, mbstate_t* _Opt restrict ps);

#else

#include_next <wchar.h>
#endif
