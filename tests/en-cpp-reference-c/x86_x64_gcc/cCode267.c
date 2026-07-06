/* Cake x86_x64_gcc */

extern double acos(double __x);
extern double cos(double __x);

double cos_pi_x_naive(double x)
{
    double pi;

    pi = acos(-1);
    return cos(pi * x);
}


extern double remquo(double __x, double __y, int * __quo);

double cos_pi_x_smart(double x)
{
    double pi;
    int extremum;
    double rem;

    pi = acos(-1);
    rem = remquo(x, 1, &extremum);
    extremum = (unsigned int)extremum % 2;
    return extremum ? -cos(pi * rem) : cos(pi * rem);
}


extern int printf(char * __format, ...);
extern int feclearexcept(int __excepts);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    int quo;

    printf("cos(pi * 0.25) = %f\n", cos_pi_x_naive(0.25));
    printf("cos(pi * 1.25) = %f\n", cos_pi_x_naive(1.25));
    printf("cos(pi * 1000000000000.25) = %f\n", cos_pi_x_naive(1000000000000.25));
    printf("cos(pi * 1000000000001.25) = %f\n", cos_pi_x_naive(1000000000001.25));
    printf("cos(pi * 1000000000000.25) = %f\n", cos_pi_x_smart(1000000000000.25));
    printf("cos(pi * 1000000000001.25) = %f\n", cos_pi_x_smart(1000000000001.25));
    feclearexcept(61);
    printf("remquo(+Inf, 1) = %.1f\n", remquo((__builtin_inff()), 1, &quo));
    if (fetestexcept(1))
    {
        puts("    FE_INVALID raised");
    }
}


