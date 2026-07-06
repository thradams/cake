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


extern int long mktime(struct tm * __tp);
extern int printf(char * __format, ...);
extern char *asctime(struct tm * __tp);

int main(void)
{
    struct tm  start;

    start.tm_sec = 0;
    start.tm_min = 0;
    start.tm_hour = 0;
    start.tm_mday = 1;
    start.tm_mon = 0;
    start.tm_year = 122;
    start.tm_wday = 0;
    start.tm_yday = 0;
    start.tm_isdst = 0;
    start.tm_gmtoff = 0;
    start.tm_zone = 0;
    mktime(&start);
    printf("%s", asctime(&start));
}


