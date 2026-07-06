/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int fetestexcept(int __excepts);

void show_fe_exceptions(void)
{
    printf("current exceptions raised: ");
    if (fetestexcept(4))
    {
        printf(" FE_DIVBYZERO");
    }
    if (fetestexcept(32))
    {
        printf(" FE_INEXACT");
    }
    if (fetestexcept(1))
    {
        printf(" FE_INVALID");
    }
    if (fetestexcept(8))
    {
        printf(" FE_OVERFLOW");
    }
    if (fetestexcept(16))
    {
        printf(" FE_UNDERFLOW");
    }
    if (fetestexcept(61) == 0)
    {
        printf(" none");
    }
    printf("\n");
}


extern double sqrt(double __x);
extern double nextafter(double __x, double __y);
extern double pow(double __x, double __y);

int main(void)
{
    show_fe_exceptions();
    printf("1.0/0.0     = %f\n", .7976931348623157E+308);
    printf("1.0/10.0    = %f\n", 0.10000000000000001);
    printf("sqrt(-1)    = %f\n", sqrt(-1));
    printf("DBL_MAX*2.0 = %f\n", .7976931348623157E+308);
    printf("nextafter(DBL_MIN/pow(2.0,52),0.0) = %.1f\n", nextafter(2.2250738585072014e-308 / pow(2.0, 52), 0.0));
    show_fe_exceptions();
    return 0;
}


