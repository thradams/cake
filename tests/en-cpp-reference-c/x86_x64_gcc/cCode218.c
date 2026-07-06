/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double atanh(double __x);
extern int *__errno_location(void);
extern int feclearexcept(int __excepts);
extern void perror(char * __s);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    printf("atanh(0) = %f\natanh(-0) = %f\n", atanh(0), atanh(-0.0));
    printf("atanh(0.9) = %f\n", atanh(0.90000000000000002));
    (*__errno_location()) = 0;
    feclearexcept(61);
    printf("atanh(-1) = %f\n", atanh(-1));
    if ((*__errno_location()) == 34)
    {
        perror("    errno == ERANGE");
    }
    if (fetestexcept(4))
    {
        puts("    FE_DIVBYZERO raised");
    }
}


