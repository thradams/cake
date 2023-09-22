#pragma once

#ifndef __STDC_VERSION_TIME_H__
#define __STDC_VERSION_TIME_H__

typedef long clock_t;

// The number of clock ticks per second
#define CLOCKS_PER_SEC  ((clock_t)1000)
#define TIME_UTC 1


//__STDC_VERSION_TIME_H__       TIME_UTC                        time_t
////NULL                          size_t                          struct timespec
////CLOCKS_PER_SEC                clock_t                         struct tm

typedef unsigned long long time_t;

struct timespec
{
    time_t tv_sec;  // Seconds - >= 0
    long   tv_nsec; // Nanoseconds - [0, 999999999]
};


struct tm
{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;

    long int tm_gmtoff;
    const char* tm_zone;

};

clock_t clock(void);
double difftime(time_t time1, time_t time0);
time_t mktime(struct tm* timeptr);
time_t timegm(struct tm* timeptr);
time_t time(time_t* timer);


int timespec_get(struct timespec* ts, int base);
int timespec_getres(struct timespec* ts, int base);
[[deprecated]] char* asctime(const struct tm* timeptr);
[[deprecated]] char* ctime(const time_t* timer);
struct tm* gmtime(const time_t* timer);
struct tm* gmtime_r(const time_t* timer, struct tm* buf);
struct tm* localtime(const time_t* timer);
struct tm* localtime_r(const time_t* timer, struct tm* buf);
size_t strftime(char* restrict s, size_t maxsize, const char* restrict format, const struct tm* restrict timeptr);


typedef int errno_t;
errno_t asctime_s(char* s, rsize_t maxsize, const struct tm* timeptr);
errno_t ctime_s(char* s, rsize_t maxsize, const time_t* timer);
struct tm* gmtime_s(const time_t* restrict timer, struct tm* restrict result);
struct tm* localtime_s(const time_t* restrict timer, struct tm* restrict result);

#endif
