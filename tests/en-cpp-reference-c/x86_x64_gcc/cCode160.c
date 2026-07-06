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


extern char *setlocale(int __category, char * __locale);
extern int long time(int long * __timer);
extern unsigned int long wcsftime(int * __s, unsigned int long __maxsize, int * __format, struct tm * __tp);
extern struct tm *localtime(int long * __timer);
extern int wprintf(int * __format, ...);

int main(void)
{
    int str[100];
    int long t;

    setlocale(6, "en_US.UTF-8");
    setlocale(1, "de_DE.utf8");
    setlocale(2, "ja_JP.utf8");
    t = time(0);
    wcsftime(str, 100, L"%A %c", localtime(&t));
    wprintf(L"Number: %.2f\nDate: %ls\n", 3.1400000000000001, str);
}


