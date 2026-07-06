/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double acos(double __x);
extern int *__errno_location(void);
extern int feclearexcept(int __excepts);
extern void perror(char * __s);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    printf("acos(-1) = %f\n", acos(-1));
    printf("acos(0.0) = %f 2*acos(0.0) = %f\n", acos(0), 2 * acos(0));
    printf("acos(0.5) = %f 3*acos(0.5) = %f\n", acos(0.5), 3 * acos(0.5));
    printf("acos(1) = %f\n", acos(1));
    (*__errno_location()) = 0;
    feclearexcept(61);
    printf("acos(1.1) = %f\n", acos(1.1000000000000001));
    if ((*__errno_location()) == 33)
    {
        perror("    errno == EDOM");
    }
    if (fetestexcept(1))
    {
        puts("    FE_INVALID raised");
    }
}


