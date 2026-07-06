/* Cake x86_x64_gcc */

extern double acos(double __x);
extern int printf(char * __format, ...);
extern double cos(double __x);
extern int feclearexcept(int __excepts);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    double pi;

    pi = acos(-1);
    printf("cos(pi/3) = %f\n", cos(pi / 3));
    printf("cos(pi/2) = %f\n", cos(pi / 2));
    printf("cos(-3*pi/4) = %f\n", cos(-3 * pi / 4));
    printf("cos(+0) = %f\n", cos(0.0));
    printf("cos(-0) = %f\n", cos(-0.0));
    feclearexcept(61);
    printf("cos(INFINITY) = %f\n", cos((__builtin_inff())));
    if (fetestexcept(1))
    {
        puts("    FE_INVALID raised");
    }
}


