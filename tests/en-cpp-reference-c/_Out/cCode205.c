//en.cppreference.com/w/c/numeric/fenv/fetestexcept.html
#include <stdio.h>
#include <math.h>
#include <fenv.h>
#include <float.h>
 
#pragma STDC FENV_ACCESS ON
 
void show_fe_exceptions(void)
{
    printf("current exceptions raised: ");
    if(fetestexcept(FE_DIVBYZERO))     printf(" FE_DIVBYZERO");
    if(fetestexcept(FE_INEXACT))       printf(" FE_INEXACT");
    if(fetestexcept(FE_INVALID))       printf(" FE_INVALID");
    if(fetestexcept(FE_OVERFLOW))      printf(" FE_OVERFLOW");
    if(fetestexcept(FE_UNDERFLOW))     printf(" FE_UNDERFLOW");
    if(fetestexcept(FE_ALL_EXCEPT)==0) printf(" none");
    printf("\n");
}
 
int main(void)
{
    /* Show default set of exception flags. */
    show_fe_exceptions();
 
    /* Perform some computations which raise exceptions. */
    printf("1.0/0.0     = %f\n", 1.0/0.0);        /* FE_DIVBYZERO            */
    printf("1.0/10.0    = %f\n", 1.0/10.0);       /* FE_INEXACT              */
    printf("sqrt(-1)    = %f\n", sqrt(-1));       /* FE_INVALID              */
    printf("DBL_MAX*2.0 = %f\n", DBL_MAX*2.0);    /* FE_INEXACT FE_OVERFLOW  */
    printf("nextafter(DBL_MIN/pow(2.0,52),0.0) = %.1f\n",
           nextafter(DBL_MIN/pow(2.0,52),0.0));   /* FE_INEXACT FE_UNDERFLOW */
    show_fe_exceptions();
 
    return 0;
}