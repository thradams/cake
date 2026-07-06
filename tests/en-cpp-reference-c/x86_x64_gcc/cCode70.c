/* Cake x86_x64_gcc */
struct timespec {
    int long tv_sec;
    int long tv_nsec;
};

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
    int long tm_gmtoff;
    char * tm_zone;
};


extern int timespec_get(struct timespec * __ts, int __base);
extern unsigned int long strftime(char * __s, unsigned int long __maxsize, char * __format, struct tm * __tp);
extern struct tm *gmtime(int long * __timer);
extern int snprintf(char * __s, unsigned int long __maxlen, char * __format, ...);
extern int vsnprintf(char * __s, unsigned int long __maxlen, char * __format, __builtin_va_list __arg);
extern int printf(char * __format, ...);

void debug_log(char * fmt, ...)
{
    struct timespec  ts;
    char time_buf[100];
    unsigned int long rc;
    __builtin_va_list args1;
    __builtin_va_list args2;
    char buf[];

    timespec_get(&ts, 1);
    rc = strftime(time_buf, 100L, "%D %T", gmtime(&ts.tv_sec));
    snprintf(time_buf + rc, 100L - rc, ".%06ld UTC", ts.tv_nsec / 1000);
    __builtin_va_start(args1, fmt);
    __builtin_va_copy(args2, args1);
    __builtin_va_end(args1);
    vsnprintf(buf, 0, fmt, args2);
    __builtin_va_end(args2);
    printf("%s [debug]: %s\n", time_buf, buf);
}


int main(void)
{
    debug_log("Logging, %d, %d, %d", 1, 2, 3);
}


