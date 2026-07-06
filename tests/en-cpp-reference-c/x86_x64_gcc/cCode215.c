/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double asinh(double __x);

int main(void)
{
    printf("asinh(1) = %f\nasinh(-1) = %f\n", asinh(1), asinh(-1));
    printf("asinh(+0) = %f\nasinh(-0) = %f\n", asinh(0.0), asinh(-0.0));
}


