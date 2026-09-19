/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#include <__cake_types.h>

#define __STDC_VERSION_STRING_H__ 202311L

typedef __cake_size_t size_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

/* copying */
void* memcpy(void* restrict s1, const void* restrict s2, size_t n);
void* memccpy(void* restrict s1, const void* restrict s2, int c, size_t n);
void* memmove(void* s1, const void* s2, size_t n);
char* strcpy(char* restrict s1, const char* restrict s2);
char* strncpy(char* restrict s1, const char* restrict s2, size_t n);
char* _Owner _Opt strdup(const char* s);
#if !defined(_WIN32)
char* _Owner _Opt strndup(const char* s, size_t n);
#endif

/* concatenation */
char* strcat(char* restrict s1, const char* restrict s2);
char* strncat(char* restrict s1, const char* restrict s2, size_t n);

/* comparison */
int memcmp(const void* s1, const void* s2, size_t n);
int strcmp(const char* s1, const char* s2);
int strcoll(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
size_t strxfrm(char* _Opt restrict s1, const char* restrict s2, size_t n);

/* search */
void* _Opt memchr(const void* s, int c, size_t n);
char* _Opt strchr(const char* s, int c);
size_t strcspn(const char* s1, const char* s2);
char* _Opt strpbrk(const char* s1, const char* s2);
char* _Opt strrchr(const char* s, int c);
size_t strspn(const char* s1, const char* s2);
char* _Opt strstr(const char* s1, const char* s2);
char* _Opt strtok(char* _Opt restrict s1, const char* restrict s2);

/* miscellaneous */
void* memset(void* s, int c, size_t n);
#if defined(__linux__)
void* memset_explicit(void* s, int c, size_t n);
#endif
char* strerror(int errnum);
size_t strlen(const char* s);
size_t strnlen(const char* s, size_t maxlen);

#if defined(__linux__) || defined(__APPLE__)
/* POSIX */
char* _Opt strtok_r(char* _Opt restrict s, const char* restrict sep, char** restrict lasts);
int strcasecmp(const char* s1, const char* s2);
int strncasecmp(const char* s1, const char* s2, size_t n);
#endif

#if defined(_WIN32)
typedef int errno_t;
typedef size_t rsize_t;
errno_t memcpy_s(void* restrict s1, rsize_t s1max, const void* restrict s2, rsize_t n);
errno_t memmove_s(void* s1, rsize_t s1max, const void* s2, rsize_t n);
errno_t strcpy_s(char* restrict s1, rsize_t s1max, const char* restrict s2);
errno_t strcat_s(char* restrict s1, rsize_t s1max, const char* restrict s2);
errno_t strncpy_s(char* restrict s1, rsize_t s1max, const char* restrict s2, rsize_t n);
char* _Opt strtok_s(char* _Opt restrict s1, const char* restrict s2, char** restrict ptr);
int _stricmp(const char* s1, const char* s2);
int _strnicmp(const char* s1, const char* s2, size_t n);
int stricmp(const char* s1, const char* s2);
int strnicmp(const char* s1, const char* s2, size_t n);
/* the msvc string.h also declares the wide string functions (corecrt_wstring.h),
   the windows sdk headers (stralign.h) count on that */
#include <wchar.h>
#endif

#else

char* _Owner _Opt strdup(const char* src);

#include_next <string.h>
#endif
