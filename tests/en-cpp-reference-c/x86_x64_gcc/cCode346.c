/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int long strtoimax(char * __nptr, char ** __endptr, int __base);
extern int *__errno_location(void);
extern char *strerror(int __errnum);

int main(void)
{
    char * endptr;

    printf("%ld\n", strtoimax(" -123junk", &endptr, 10));
    printf("%ld\n", strtoimax("11111111", &endptr, 2));
    printf("%ld\n", strtoimax("XyZ", &endptr, 36));
    printf("%ld\n", strtoimax("010", &endptr, 0));
    printf("%ld\n", strtoimax("10", &endptr, 0));
    printf("%ld\n", strtoimax("0x10", &endptr, 0));
    (*__errno_location()) = 0;
    printf("%ld\n", strtoimax("9223372036854775808", &endptr, 10));
    printf("%s\n", strerror((*__errno_location())));
    return 0;
}


