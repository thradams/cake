#ifndef TIME_H
#define TIME_H

#include <stddef.h>

typedef long time_t;
typedef long clock_t;
typedef struct tm {
    int tm_sec;   /* seconds 0-60 */
    int tm_min;   /* minutes 0-59 */
    int tm_hour;  /* hours 0-23 */
    int tm_mday;  /* day 1-31 */
    int tm_mon;   /* month 0-11 */
    int tm_year;  /* years since 1900 */
    int tm_wday;  /* day of week 0-6 */
    int tm_yday;  /* day of year 0-365 */
    int tm_isdst; /* daylight saving time flag */
} tm;

#define CLOCKS_PER_SEC 1000000

clock_t clock(void);
double difftime(time_t time1, time_t time0);
time_t mktime(tm *timeptr);
time_t time(time_t *timer);
char *asctime(const tm *timeptr);
char *ctime(const time_t *timer);
tm *gmtime(const time_t *timer);
tm *localtime(const time_t *timer);
char *asctime_r(const tm *timeptr, char *buf);
char *ctime_r(const time_t *timer, char *buf);
tm *gmtime_r(const time_t *timer, tm *res);
tm *localtime_r(const time_t *timer, tm *res);
size_t strftime(char *restrict s, size_t max, const char *restrict format, const tm *restrict timeptr);

/* C23 addition: timespec_get */
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};
int timespec_get(struct timespec *ts, int base);

#endif /* TIME_H */
