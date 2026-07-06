/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double copysign(double __x, double __y);

int main(void)
{
    printf("copysign(1.0,+2.0)      = %+.1f\n", copysign(1.0, 2.0));
    printf("copysign(1.0,-2.0)      = %+.1f\n", copysign(1.0, -2.0));
    printf("copysign(INFINITY,-2.0) = %f\n", copysign((__builtin_inff()), -2.0));
    printf("copysign(NAN,-2.0)      = %f\n", copysign((__builtin_nanf("")), -2.0));
}


