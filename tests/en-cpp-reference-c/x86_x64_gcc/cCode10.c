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
    int long __tm_gmtoff;
    char * __tm_zone;
};


extern int long time(int long * __timer);
extern int printf(char * __format, ...);
extern char *asctime(struct tm * __tp);
extern struct tm *gmtime(int long * __timer);
extern struct tm *localtime(int long * __timer);
extern int putenv(char * __string);

int main(void)
{
    int long t;

    t = time(0);
    printf("UTC:       %s", asctime(gmtime(&t)));
    printf("local:     %s", asctime(localtime(&t)));
    putenv("TZ=Asia/Singapore");
    printf("Singapore: %s", asctime(localtime(&t)));
}


