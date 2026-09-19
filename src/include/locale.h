/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#include <__cake_types.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

#if defined(__linux__)
#define LC_CTYPE    0
#define LC_NUMERIC  1
#define LC_TIME     2
#define LC_COLLATE  3
#define LC_MONETARY 4
#define LC_MESSAGES 5
#define LC_ALL      6
#else
#define LC_ALL      0
#define LC_COLLATE  1
#define LC_CTYPE    2
#define LC_MONETARY 3
#define LC_NUMERIC  4
#define LC_TIME     5
#endif

/* the layout must match the libc of the target */
struct lconv
{
    char* decimal_point;
    char* thousands_sep;
    char* grouping;
    char* int_curr_symbol;
    char* currency_symbol;
    char* mon_decimal_point;
    char* mon_thousands_sep;
    char* mon_grouping;
    char* positive_sign;
    char* negative_sign;
    char  int_frac_digits;
    char  frac_digits;
    char  p_cs_precedes;
    char  p_sep_by_space;
    char  n_cs_precedes;
    char  n_sep_by_space;
    char  p_sign_posn;
    char  n_sign_posn;
#if defined(_WIN32)
    __cake_wchar_t* _W_decimal_point;
    __cake_wchar_t* _W_thousands_sep;
    __cake_wchar_t* _W_int_curr_symbol;
    __cake_wchar_t* _W_currency_symbol;
    __cake_wchar_t* _W_mon_decimal_point;
    __cake_wchar_t* _W_mon_thousands_sep;
    __cake_wchar_t* _W_positive_sign;
    __cake_wchar_t* _W_negative_sign;
#elif defined(__APPLE__)
    char  int_p_cs_precedes;
    char  int_n_cs_precedes;
    char  int_p_sep_by_space;
    char  int_n_sep_by_space;
    char  int_p_sign_posn;
    char  int_n_sign_posn;
#else
    char  int_p_cs_precedes;
    char  int_p_sep_by_space;
    char  int_n_cs_precedes;
    char  int_n_sep_by_space;
    char  int_p_sign_posn;
    char  int_n_sign_posn;
#endif
};

char* _Opt setlocale(int category, const char* _Opt locale);
struct lconv* localeconv(void);

#else
#include_next <locale.h>
#endif
