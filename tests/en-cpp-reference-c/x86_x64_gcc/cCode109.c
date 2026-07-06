/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double cbrt(double __x);
extern float cbrtf(float __x);

int main(void)
{
    double x;
    float y;

    x = 8.0;
    y = 3.375;
    printf("cbrt(8.0) = %f\n", cbrt(x));
    printf("cbrtf(3.375) = %f\n", cbrtf(3.375f));
}


