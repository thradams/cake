/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double sqrt(double __x);
extern int *__errno_location(void);
extern int feclearexcept(int __excepts);
extern void perror(char * __s);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    printf("sqrt(100) = %f\n", sqrt(100));
    printf("sqrt(2) = %f\n", sqrt(2));
    printf("golden ratio = %f\n", (1 + sqrt(5)) / 2);
    printf("sqrt(-0) = %f\n", sqrt(-0.0));
    (*__errno_location()) = 0;
    feclearexcept(61);
    printf("sqrt(-1.0) = %f\n", sqrt(-1));
    if ((*__errno_location()) == 33)
    {
        perror("    errno == EDOM");
    }
    if (fetestexcept(1))
    {
        puts("    FE_INVALID was raised");
    }
}


