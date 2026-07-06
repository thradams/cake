/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double trunc(double __x);

int main(void)
{
    printf("trunc(+2.7) = %+.1f\n", trunc(2.7000000000000002));
    printf("trunc(-2.7) = %+.1f\n", trunc(-2.7000000000000002));
    printf("trunc(-0.0) = %+.1f\n", trunc(-0.0));
    printf("trunc(-Inf) = %+f\n", trunc(-(__builtin_inff())));
}


