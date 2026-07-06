/* Cake x86_x64_gcc */
struct __tag5 {
    int quot;
    int rem;
};


void reverse(char * first, char * last)
{
    for (--last; first < last; ++first, --last)
    {
        char c;

        c = *last;
        *last = *first;
        *first = c;
    }
}


extern struct __tag5 div(int __numer, int __denom);
extern int abs(int __x);

char *my_itoa(int n, int base, char * buf, unsigned int long buf_size)
{
    struct __tag5  dv;
    char * p;

    ;
    dv.quot = n;
    dv.rem = 0;
    p = buf;
    do
    {
        if (!--buf_size)
        {
            return (*buf = 0), buf;
        }
        dv = div(dv.quot, base);
        *p++ = "0123456789abcdef"[abs(dv.rem)];
    }
    while (dv.quot);
    if (n < 0)
    {
        *p++ = 45;
    }
    *p = 0;
    reverse(buf, p);
    return buf;
}


extern int printf(char * __format, ...);

int main(void)
{
    char buf[16];

    printf("%s\n", my_itoa(0, 2, buf, 16L));
    printf("%s\n", my_itoa(7, 3, buf, 16L));
    printf("%s\n", my_itoa(12346, 10, buf, 16L));
    printf("%s\n", my_itoa(-12346, 10, buf, 16L));
    printf("%s\n", my_itoa(-42, 2, buf, 16L));
    printf("%s\n", my_itoa(2147483647, 16, buf, 16L));
    printf("%s\n", my_itoa(-2147483648, 16, buf, 16L));
}


