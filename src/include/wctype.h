/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#include <__cake_types.h>

#define __STDC_VERSION_WCTYPE_H__ 202311L

typedef __cake_wint_t wint_t;
#if defined(_WIN32)
typedef unsigned short wctype_t;
typedef __cake_wchar_t wctrans_t;
#elif defined(__APPLE__)
typedef unsigned int wctype_t;
typedef int wctrans_t;
#else
typedef unsigned long wctype_t;
typedef const int* wctrans_t;
#endif

#define WEOF ((wint_t)-1)

/* wide character classification */
int iswalnum(wint_t wc);
int iswalpha(wint_t wc);
int iswblank(wint_t wc);
int iswcntrl(wint_t wc);
int iswdigit(wint_t wc);
int iswgraph(wint_t wc);
int iswlower(wint_t wc);
int iswprint(wint_t wc);
int iswpunct(wint_t wc);
int iswspace(wint_t wc);
int iswupper(wint_t wc);
int iswxdigit(wint_t wc);

/* extensible wide character classification */
int iswctype(wint_t wc, wctype_t desc);
wctype_t wctype(const char* property);

/* wide character case mapping */
wint_t towlower(wint_t wc);
wint_t towupper(wint_t wc);
wint_t towctrans(wint_t wc, wctrans_t desc);
wctrans_t wctrans(const char* property);

#else
#include_next <wctype.h>
#endif
