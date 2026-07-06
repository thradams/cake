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
extern int printf(char * __format, ...);

int main(void)
{
    struct timespec  ts;
    char buff[100];

    timespec_get(&ts, 1);
    strftime(buff, 100L, "%D %T", gmtime(&ts.tv_sec));
    printf("Current time: %s.%09ld UTC\n", buff, ts.tv_nsec);
}


