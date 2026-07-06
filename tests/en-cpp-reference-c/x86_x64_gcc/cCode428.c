/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

void simple_printf(char * fmt, ...)
{
    __builtin_va_list args;

    __builtin_va_start(args, fmt);
    while (*fmt != 0)
    {
        if (*fmt == 100)
        {
            int i;

            i = __builtin_va_arg(args, int);
            printf("%d\n", i);
        }
        else
        {
            if (*fmt == 99)
            {
                int c;

                c = __builtin_va_arg(args, int);
                printf("%c\n", c);
            }
            else
            {
                if (*fmt == 102)
                {
                    double d;

                    d = __builtin_va_arg(args, double);
                    printf("%f\n", d);
                }
            }
        }
        ++fmt;
    }
    __builtin_va_end(args);
}


int main(void)
{
    simple_printf("dcff", 3, 97, 1.9990000000000001, 42.5);
}


