/* Cake 0.12.26 x86_msvc */
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
};


unsigned int __cdecl strftime(char * _Buffer, unsigned int _SizeInBytes, char * _Format, struct tm * _Tm);
int __cdecl puts(char * _Buffer);

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
    strftime(buff, 70, "%A %c", &my_time);
    puts(buff);
}


