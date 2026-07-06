/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double ceil(double __x);

int main(void)
{
    printf("ceil(+2.4) = %+.1f\n", ceil(2.3999999999999999));
    printf("ceil(-2.4) = %+.1f\n", ceil(-2.3999999999999999));
    printf("ceil(-0.0) = %+.1f\n", ceil(-0.0));
    printf("ceil(-Inf) = %+f\n", ceil(-(__builtin_inff())));
}


