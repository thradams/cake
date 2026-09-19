/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#include <__cake_types.h>

#define __STDC_VERSION_UCHAR_H__ 202311L

typedef __cake_size_t size_t;

typedef __cake_mbstate_t mbstate_t;

typedef unsigned char  char8_t;
typedef unsigned short char16_t;
typedef unsigned int   char32_t;

#if defined(__linux__)
size_t mbrtoc8(char8_t* _Opt restrict pc8, const char* _Opt restrict s, size_t n, mbstate_t* _Opt restrict ps);
size_t c8rtomb(char* _Opt restrict s, char8_t c8, mbstate_t* _Opt restrict ps);
#endif
size_t mbrtoc16(char16_t* _Opt restrict pc16, const char* _Opt restrict s, size_t n, mbstate_t* _Opt restrict ps);
size_t c16rtomb(char* _Opt restrict s, char16_t c16, mbstate_t* _Opt restrict ps);
size_t mbrtoc32(char32_t* _Opt restrict pc32, const char* _Opt restrict s, size_t n, mbstate_t* _Opt restrict ps);
size_t c32rtomb(char* _Opt restrict s, char32_t c32, mbstate_t* _Opt restrict ps);

#else
#include_next <uchar.h>
#endif
