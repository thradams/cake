/* Cake x86_x64_gcc */

extern int wprintf(int * __format, ...);
extern int long wcstoimax(int * __nptr, int ** __endptr, int __base);
extern int *__errno_location(void);
extern char *strerror(int __errnum);

int main(void)
{
    int * endptr;

    wprintf(L"%ld\n", wcstoimax(L" -123junk", &endptr, 10));
    wprintf(L"%ld\n", wcstoimax(L"11111111", &endptr, 2));
    wprintf(L"%ld\n", wcstoimax(L"XyZ", &endptr, 36));
    wprintf(L"%ld\n", wcstoimax(L"010", &endptr, 0));
    wprintf(L"%ld\n", wcstoimax(L"10", &endptr, 0));
    wprintf(L"%ld\n", wcstoimax(L"0x10", &endptr, 0));
    (*__errno_location()) = 0;
    wprintf(L"%ld\n", wcstoimax(L"9223372036854775808", &endptr, 10));
    wprintf(L"%s\n", strerror((*__errno_location())));
}


