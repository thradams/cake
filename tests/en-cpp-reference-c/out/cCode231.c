//en.cppreference.com/w/c/numeric/math/fabs.html
#include <math.h>
#include <stdio.h>
 
#define PI 3.14159
 
// This numerical integration assumes all area is positive.
double integrate(double f(double),
                 double a, double b, // assume a < b
                 unsigned steps) // assume steps > 0
{
    const double dx = (b - a) / steps;
    double sum = 0.0;
    for (double x = a; x < b; x += dx)
        sum += fabs(f(x));
    return dx * sum;
}
 
int main(void)
{
    printf("fabs(+3) = %f\n", fabs(+3.0));
    printf("fabs(-3) = %f\n", fabs(-3.0));
    // special values
    printf("fabs(-0) = %f\n", fabs(-0.0));
    printf("fabs(-Inf) = %f\n", fabs(-INFINITY));
 
    printf("Area under sin(x) in [-PI, PI] = %f\n", integrate(sin, -PI, PI, 5101));
}