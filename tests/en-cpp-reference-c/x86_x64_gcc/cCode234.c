/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double floor(double __x);

int main(void)
{
    printf("floor(+2.7) = %+.1f\n", floor(2.7000000000000002));
    printf("floor(-2.7) = %+.1f\n", floor(-2.7000000000000002));
    printf("floor(-0.0) = %+.1f\n", floor(-0.0));
    printf("floor(-Inf) = %+f\n", floor(-(__builtin_inff())));
}


