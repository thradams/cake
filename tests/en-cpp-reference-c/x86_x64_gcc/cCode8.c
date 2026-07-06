/* Cake x86_x64_gcc */
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


extern int long time(int long * __timer);
extern struct tm *localtime(int long * __timer);
extern double difftime(int long __time1, int long __time0);
extern int long mktime(struct tm * __tp);
extern int printf(char * __format, ...);

int main(void)
{
    int long now;
    struct tm  beg;
    double seconds;

    now = time(0);
    beg = *localtime(&now);
    beg.tm_hour = 0, beg.tm_min = 0, beg.tm_sec = 0, beg.tm_mday = 1;
    seconds = difftime(now, mktime(&beg));
    printf("%.f seconds have passed since the beginning of the month.\n", seconds);
    return 0;
}


