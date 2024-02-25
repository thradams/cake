//en.cppreference.com/w/c/numeric/fenv/FE_exceptions.html
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <fenv.h>
 
#pragma STDC FENV_ACCESS ON
void show_fe_exceptions(void)
{
    printf("exceptions raised:");
    if(fetestexcept(FE_DIVBYZERO)) printf(" FE_DIVBYZERO");
    if(fetestexcept(FE_INEXACT))   printf(" FE_INEXACT");
    if(fetestexcept(FE_INVALID))   printf(" FE_INVALID");
    if(fetestexcept(FE_OVERFLOW))  printf(" FE_OVERFLOW");
    if(fetestexcept(FE_UNDERFLOW)) printf(" FE_UNDERFLOW");
    feclearexcept(FE_ALL_EXCEPT);
    printf("\n");
}
 
int main(void)
{
    printf("MATH_ERREXCEPT is %s\n",
           math_errhandling & MATH_ERREXCEPT ? "set" : "not set");
 
    printf("0.0/0.0 = %f\n", 0.0/0.0);
    show_fe_exceptions();
 
    printf("1.0/0.0 = %f\n", 1.0/0.0);
    show_fe_exceptions();
 
    printf("1.0/10.0 = %f\n", 1.0/10.0);
    show_fe_exceptions();
 
    printf("sqrt(-1) = %f\n", sqrt(-1));
    show_fe_exceptions();
 
    printf("DBL_MAX*2.0 = %f\n", DBL_MAX*2.0);
    show_fe_exceptions();
 
    printf("nextafter(DBL_MIN/pow(2.0,52),0.0) = %.1f\n",
                      nextafter(DBL_MIN/pow(2.0,52),0.0));
    show_fe_exceptions();
}