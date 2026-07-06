/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double tanh(double __x);
extern double sinh(double __x);
extern double cosh(double __x);

int main(void)
{
    printf("tanh(1) = %f\ntanh(-1) = %f\n", tanh(1), tanh(-1));
    printf("tanh(0.1)*sinh(0.2)-cosh(0.2) = %f\n", tanh(0.10000000000000001) * sinh(0.20000000000000001) - cosh(0.20000000000000001));
    printf("tanh(+0) = %f\ntanh(-0) = %f\n", tanh(0.0), tanh(-0.0));
}


