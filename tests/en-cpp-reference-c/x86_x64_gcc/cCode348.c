/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int *__errno_location(void);
extern int long strtol(char * __nptr, char ** __endptr, int __base);
extern int putchar(int __c);

int main(void)
{
    char * p;

    p = "10 200000000000000000000000000000 30 -40 junk";
    printf("Parsing '%s':\n", p);
    for (; ; )
    {
        char * end;
        long i;
        unsigned char  range_error;

        (*__errno_location()) = 0;
        i = strtol(p, &end, 10);
        if (p == end)
        {
            break;
        }
        range_error = (*__errno_location()) == 34;
        printf("Extracted '%.*s', strtol returned %ld.", (int)(end - p), p, i);
        p = end;
        if (range_error)
        {
            printf("\n --> Range error occurred.");
        }
        putchar(10);
    }
    printf("Unextracted leftover: '%s'\n\n", p);
    printf("\"1010\" in binary  --> %ld\n", strtol("1010", 0, 2));
    printf("\"12\"   in octal   --> %ld\n", strtol("12", 0, 8));
    printf("\"A\"    in hex     --> %ld\n", strtol("A", 0, 16));
    printf("\"junk\" in base-36 --> %ld\n", strtol("junk", 0, 36));
    printf("\"012\"  in auto-detected base --> %ld\n", strtol("012", 0, 0));
    printf("\"0xA\"  in auto-detected base --> %ld\n", strtol("0xA", 0, 0));
    printf("\"junk\" in auto-detected base --> %ld\n", strtol("junk", 0, 0));
}


