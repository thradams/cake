/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double cbrt(double __x);
extern double pow(double __x, double __y);

int main(void)
{
    printf("Normal use:\ncbrt(729)      = %f\n", cbrt(729));
    printf("cbrt(-0.125)   = %f\n", cbrt(-0.125));
    printf("Special values:\ncbrt(-0)       = %f\n", cbrt(-0.0));
    printf("cbrt(+inf)     = %f\n", cbrt((__builtin_inff())));
    printf("Accuracy:\ncbrt(343)      = %.*f\n", 17, cbrt(343));
    printf("pow(343,1.0/3) = %.*f\n", 17, pow(343, 0.33333333333333331));
}


