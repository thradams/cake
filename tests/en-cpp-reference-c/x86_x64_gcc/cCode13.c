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
extern int printf(char * __format, ...);
extern char *asctime(struct tm * __tp);
extern struct tm *gmtime(int long * __timer);

int main(void)
{
    int long result;

    result = time(0);
    if (result != -1L)
    {
        printf("The current time is %s(%jd seconds since the Epoch)\n", asctime(gmtime(&result)), (int long)result);
    }
}


