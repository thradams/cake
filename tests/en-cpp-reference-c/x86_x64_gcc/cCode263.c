/* Cake x86_x64_gcc */

extern int fesetround(int __rounding_direction);
extern int printf(char * __format, ...);
extern double nearbyint(double __x);

int main(void)
{
    fesetround(0);
    printf("rounding to nearest:\nnearbyint(+2.3) = %+.1f  ", nearbyint(2.2999999999999998));
    printf("nearbyint(+2.5) = %+.1f  ", nearbyint(2.5));
    printf("nearbyint(+3.5) = %+.1f\n", nearbyint(3.5));
    printf("nearbyint(-2.3) = %+.1f  ", nearbyint(-2.2999999999999998));
    printf("nearbyint(-2.5) = %+.1f  ", nearbyint(-2.5));
    printf("nearbyint(-3.5) = %+.1f\n", nearbyint(-3.5));
    fesetround(1024);
    printf("rounding down: \nnearbyint(+2.3) = %+.1f  ", nearbyint(2.2999999999999998));
    printf("nearbyint(+2.5) = %+.1f  ", nearbyint(2.5));
    printf("nearbyint(+3.5) = %+.1f\n", nearbyint(3.5));
    printf("nearbyint(-2.3) = %+.1f  ", nearbyint(-2.2999999999999998));
    printf("nearbyint(-2.5) = %+.1f  ", nearbyint(-2.5));
    printf("nearbyint(-3.5) = %+.1f\n", nearbyint(-3.5));
    printf("nearbyint(-0.0) = %+.1f\n", nearbyint(-0.0));
    printf("nearbyint(-Inf) = %+.1f\n", nearbyint(-(__builtin_inff())));
}


