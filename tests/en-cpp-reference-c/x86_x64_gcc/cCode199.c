/* Cake x86_x64_gcc */

extern int feclearexcept(int __excepts);
extern double sqrt(double __x);
extern int fetestexcept(int __excepts);
extern double frexp(double __x, int * __exponent);
extern double fabs(double __x);
extern double scalbn(double __x, int __n);

double hypot_demo(double a, double b)
{
    int range_problem;
    double result;
    int a_exponent;
    int b_exponent;
    double a_scaled;
    double b_scaled;

    range_problem = 24;
    feclearexcept(24);
    result = sqrt(a * a + b * b);
    if (!fetestexcept(24))
    {
        return result;
    }
    frexp(a, &a_exponent);
    frexp(b, &b_exponent);
    if (a_exponent - b_exponent > 1024)
    {
        return fabs(a) + fabs(b);
    }
    a_scaled = scalbn(a, -a_exponent);
    b_scaled = scalbn(b, -a_exponent);
    result = sqrt(a_scaled * a_scaled + b_scaled * b_scaled);
    return scalbn(result, a_exponent);
}


extern int printf(char * __format, ...);

int main(void)
{
    printf("hypot(%f, %f) = %f\n", 3.0, 4.0, hypot_demo(3.0, 4.0));
    printf("hypot(%e, %e) = %e\n", 8.9884656743115785e+307, 8.9884656743115785e+307, hypot_demo(8.9884656743115785e+307, 8.9884656743115785e+307));
    return 0;
}


