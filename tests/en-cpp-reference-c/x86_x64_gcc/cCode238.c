/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double fmod(double __x, double __y);
extern int feclearexcept(int __excepts);
extern int fetestexcept(int __excepts);
extern int puts(char * __s);

int main(void)
{
    printf("fmod(+5.1, +3.0) = %.1f\n", fmod(5.0999999999999996, 3));
    printf("fmod(-5.1, +3.0) = %.1f\n", fmod(-5.0999999999999996, 3));
    printf("fmod(+5.1, -3.0) = %.1f\n", fmod(5.0999999999999996, -3));
    printf("fmod(-5.1, -3.0) = %.1f\n", fmod(-5.0999999999999996, -3));
    printf("fmod(+0.0, 1.0) = %.1f\n", fmod(0, 1));
    printf("fmod(-0.0, 1.0) = %.1f\n", fmod(-0.0, 1));
    printf("fmod(+5.1, Inf) = %.1f\n", fmod(5.0999999999999996, (__builtin_inff())));
    feclearexcept(61);
    printf("fmod(+5.1, 0) = %.1f\n", fmod(5.0999999999999996, 0));
    if (fetestexcept(1))
    {
        puts("    FE_INVALID raised");
    }
}


