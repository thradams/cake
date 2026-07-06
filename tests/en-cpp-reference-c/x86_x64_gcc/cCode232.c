/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double fdim(double __x, double __y);
extern int *__errno_location(void);
extern int feclearexcept(int __excepts);
extern void perror(char * __s);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    printf("fdim(4, 1) = %f, fdim(1, 4)=%f\n", fdim(4, 1), fdim(1, 4));
    printf("fdim(4,-1) = %f, fdim(1,-4)=%f\n", fdim(4, -1), fdim(1, -4));
    (*__errno_location()) = 0;
    feclearexcept(61);
    printf("fdim(1e308, -1e308) = %f\n", fdim(1e+308, -1e+308));
    if ((*__errno_location()) == 34)
    {
        perror("    errno == ERANGE");
    }
    if (fetestexcept(8))
    {
        puts("    FE_OVERFLOW raised");
    }
}


