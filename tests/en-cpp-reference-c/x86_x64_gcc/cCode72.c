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
extern int vswprintf(int * __s, unsigned int long __n, int * __format, __builtin_va_list __arg);
extern int wprintf(int * __format, ...);

void debug_wlog(int * fmt, ...)
{
    struct timespec  ts;
    char time_buf[100];
    unsigned int long rc;
    __builtin_va_list args;
    int buf[1024];
    int rc2;

    timespec_get(&ts, 1);
    rc = strftime(time_buf, 100L, "%D %T", gmtime(&ts.tv_sec));
    snprintf(time_buf + rc, 100L - rc, ".%06ld UTC", ts.tv_nsec / 1000);
    __builtin_va_start(args, fmt);
    rc2 = vswprintf(buf, 1024L, fmt, args);
    __builtin_va_end(args);
    if (rc2 > 0)
    {
        wprintf(L"%s [debug]: %ls\n", time_buf, buf);
    }
    else
    {
        wprintf(L"%s [debug]: (string too long)\n", time_buf);
    }
}


extern char *setlocale(int __category, char * __locale);

int main(void)
{
    setlocale(6, "");
    debug_wlog(L"Logging, %d, %d, %d", 1, 2, 3);
}


