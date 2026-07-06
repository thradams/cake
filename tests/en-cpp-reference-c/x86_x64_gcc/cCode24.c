/* Cake x86_x64_gcc */

extern int *__errno_location(void);
extern int printf(char * __format, ...);
extern double log(double __x);
extern char *strerror(int __errnum);

int main(void)
{
    (*__errno_location()) = 0;
    printf("log(-1.0) = %f\n", log(-1.0));
    printf("%s\n\n", strerror((*__errno_location())));
    (*__errno_location()) = 0;
    printf("log(0.0)  = %f\n", log(0.0));
    printf("%s\n", strerror((*__errno_location())));
}


