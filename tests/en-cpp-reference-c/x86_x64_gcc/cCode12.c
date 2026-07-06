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


extern unsigned int long strftime(char * __s, unsigned int long __maxsize, char * __format, struct tm * __tp);
extern int puts(char * __s);
extern char *setlocale(int __category, char * __locale);

int main(void)
{
    char buff[70];
    struct tm  my_time;

    my_time.tm_sec = 20;
    my_time.tm_min = 10;
    my_time.tm_hour = 8;
    my_time.tm_mday = 9;
    my_time.tm_mon = 9;
    my_time.tm_year = 112;
    my_time.tm_wday = 0;
    my_time.tm_yday = 0;
    my_time.tm_isdst = 0;
    my_time.tm_gmtoff = 0;
    my_time.tm_zone = 0;
    if (strftime(buff, 70L, "%A %c", &my_time))
    {
        puts(buff);
    }
    else
    {
        puts("strftime failed");
    }
    setlocale(2, "el_GR.utf8");
    if (strftime(buff, 70L, "%A %c", &my_time))
    {
        puts(buff);
    }
    else
    {
        puts("strftime failed");
    }
}


