/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double exp(double __x);
extern int *__errno_location(void);
extern int feclearexcept(int __excepts);
extern void perror(char * __s);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    printf("exp(1) = %f\n", exp(1));
    printf("FV of $100, continuously compounded at 3%% for 1 year = %f\n", 100 * exp(0.029999999999999999));
    printf("exp(-0) = %f\n", exp(-0.0));
    printf("exp(-Inf) = %f\n", exp(-(__builtin_inff())));
    (*__errno_location()) = 0;
    feclearexcept(61);
    printf("exp(710) = %f\n", exp(710));
    if ((*__errno_location()) == 34)
    {
        perror("    errno == ERANGE");
    }
    if (fetestexcept(8))
    {
        puts("    FE_OVERFLOW raised");
    }
}


