/*
 *  This file is part of cake compiler
 *  https://github.com/thradams/cake
*/

#ifdef CAKE_HEADERS

#pragma once

#include <__cake_types.h>

#define __STDC_VERSION_TIME_H__ 202311L

typedef __cake_size_t size_t;
typedef __cake_time_t time_t;
typedef __cake_clock_t clock_t;

#ifndef NULL
#define NULL ((void*)0)
#endif

#if defined(_WIN32)
#define CLOCKS_PER_SEC 1000
#else
#define CLOCKS_PER_SEC 1000000
#endif

#define TIME_UTC 1
#if defined(__linux__)
/* the other C23 bases are only honoured by glibc >= 2.34 */
#define TIME_MONOTONIC 2
#define TIME_ACTIVE 3
#define TIME_THREAD_ACTIVE 4
#endif

struct tm
{
    int tm_sec;   /* seconds after the minute [0, 60] */
    int tm_min;   /* minutes after the hour [0, 59] */
    int tm_hour;  /* hours since midnight [0, 23] */
    int tm_mday;  /* day of the month [1, 31] */
    int tm_mon;   /* months since January [0, 11] */
    int tm_year;  /* years since 1900 */
    int tm_wday;  /* days since Sunday [0, 6] */
    int tm_yday;  /* days since January 1 [0, 365] */
    int tm_isdst; /* Daylight Saving Time flag */
#if defined(__linux__) || defined(__APPLE__)
    long tm_gmtoff;
    const char* tm_zone;
#endif
};

struct timespec
{
    time_t tv_sec;
    long tv_nsec;
};

#if defined(_WIN32)

/* the msvc CRT exports only the 64 bits versions, time() etc. are inline
   wrappers in its headers */
clock_t clock(void);
double _difftime64(time_t time1, time_t time0);
time_t _mktime64(struct tm* timeptr);
time_t _mkgmtime64(struct tm* timeptr);
time_t _time64(time_t* _Opt timer);
int _timespec64_get(struct timespec* ts, int base);
char* _Opt _ctime64(const time_t* timer);
struct tm* _Opt _gmtime64(const time_t* timer);
struct tm* _Opt _localtime64(const time_t* timer);
char* _Opt asctime(const struct tm* timeptr);
size_t strftime(char* restrict s, size_t maxsize, const char* restrict format, const struct tm* restrict timeptr);

#define difftime     _difftime64
#define mktime       _mktime64
#define timegm       _mkgmtime64
#define time         _time64
#define timespec_get _timespec64_get
#define ctime        _ctime64
#define gmtime       _gmtime64
#define localtime    _localtime64

#else

/* time manipulation */
clock_t clock(void);
double difftime(time_t time1, time_t time0);
time_t mktime(struct tm* timeptr);
time_t timegm(struct tm* timeptr);
time_t time(time_t* _Opt timer);
int timespec_get(struct timespec* ts, int base);
#if defined(__linux__)
int timespec_getres(struct timespec* ts, int base);
#endif

/* time conversion */
char* _Opt asctime(const struct tm* timeptr);
char* _Opt ctime(const time_t* timer);
struct tm* _Opt gmtime(const time_t* timer);
struct tm* _Opt localtime(const time_t* timer);
size_t strftime(char* restrict s, size_t maxsize, const char* restrict format, const struct tm* restrict timeptr);

#if defined(__linux__) || defined(__APPLE__)
/* POSIX */
struct tm* _Opt gmtime_r(const time_t* timer, struct tm* buf);
struct tm* _Opt localtime_r(const time_t* timer, struct tm* buf);
char* _Opt asctime_r(const struct tm* restrict tm, char* restrict buf);
char* _Opt ctime_r(const time_t* clock, char* buf);
int nanosleep(const struct timespec* req, struct timespec* _Opt rem);
#endif

#endif

#else

/* time() must be declared before the system header; the first declaration wins */
#if defined(__APPLE__)
#include <sys/_types.h>
#include <sys/_types/_time_t.h>
time_t time(time_t* _Opt timer);
#elif defined(__linux__)
#include <bits/types/time_t.h>
time_t time(time_t* _Opt timer);
#elif defined(_WIN32)
/* the UCRT's time() is an inline wrapper over _time64 whose linkage is
   _CRT_NONSTANDARD_STATIC (static unless _STATIC_INLINE_UCRT_FUNCTIONS is 0;
   always static in older SDKs) - this declaration must match it */
#include <corecrt.h>
#ifdef _CRT_NONSTANDARD_STATIC
_CRT_NONSTANDARD_STATIC time_t __CRTDECL time(time_t* _Opt timer);
#else
static time_t __CRTDECL time(time_t* _Opt timer);
#endif
#endif

#include_next <time.h>
#endif
