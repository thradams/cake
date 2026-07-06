/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int feclearexcept(int __excepts);
extern int *__errno_location(void);
extern double log(double __x);
extern void perror(char * __s);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    printf("MATH_ERRNO is %s\n", 1 ? "set" : "not set");
    printf("MATH_ERREXCEPT is %s\n", 2 ? "set" : "not set");
    feclearexcept(61);
    (*__errno_location()) = 0;
    printf("log(0) = %f\n", log(0));
    if ((*__errno_location()) == 34)
    {
        perror("errno == ERANGE");
    }
    if (fetestexcept(4))
    {
        puts("FE_DIVBYZERO (pole error) reported");
    }
}


