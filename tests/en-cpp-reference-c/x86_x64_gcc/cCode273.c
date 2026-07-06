/* Cake x86_x64_gcc */

extern double acos(double __x);
extern int printf(char * __format, ...);
extern double sin(double __x);
extern int feclearexcept(int __excepts);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    double pi;

    pi = acos(-1);
    printf("sin(pi/6) = %f\n", sin(pi / 6));
    printf("sin(pi/2) = %f\n", sin(pi / 2));
    printf("sin(-3*pi/4) = %f\n", sin(-3 * pi / 4));
    printf("sin(+0) = %f\n", sin(0.0));
    printf("sin(-0) = %f\n", sin(-0.0));
    feclearexcept(61);
    printf("sin(INFINITY) = %f\n", sin((__builtin_inff())));
    if (fetestexcept(1))
    {
        puts("    FE_INVALID raised");
    }
}


