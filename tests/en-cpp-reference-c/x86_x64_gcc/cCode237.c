/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern double fmin(double __x, double __y);

int main(void)
{
    printf("fmin(2,1)    = %f\n", fmin(2, 1));
    printf("fmin(-Inf,0) = %f\n", fmin(-(__builtin_inff()), 0));
    printf("fmin(NaN,-1) = %f\n", fmin((__builtin_nanf("")), -1));
}


