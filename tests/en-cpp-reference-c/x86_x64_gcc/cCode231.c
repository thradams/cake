/* Cake x86_x64_gcc */

extern double fabs(double __x);
double f(double);

double integrate(double f(double), double a, double b, unsigned int steps)
{
    double dx;
    double sum;

    dx = (b - a) / steps;
    sum = 0.0;
    {
        double x;

        x = a;
        for (; x < b; x += dx)
        sum += fabs(f(x));
    }
    return dx * sum;
}


extern int printf(char * __format, ...);
extern double sin(double __x);

int main(void)
{
    printf("fabs(+3) = %f\n", fabs(3.0));
    printf("fabs(-3) = %f\n", fabs(-3.0));
    printf("fabs(-0) = %f\n", fabs(-0.0));
    printf("fabs(-Inf) = %f\n", fabs(-(__builtin_inff())));
    printf("Area under sin(x) in [-PI, PI] = %f\n", integrate(sin, -3.1415899999999999, 3.1415899999999999, 5101));
}


