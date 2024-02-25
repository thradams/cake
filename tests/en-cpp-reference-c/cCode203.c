//en.cppreference.com/w/c/numeric/fenv/feraiseexcept.html
#include <stdio.h>
#include <fenv.h>
 
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
    feclearexcept(FE_ALL_EXCEPT);
    printf("\n");
}
 
double some_computation(void)
{
    /* Computation reaches a state that causes overflow. */
    int r = feraiseexcept(FE_OVERFLOW | FE_INEXACT);
    printf("feraiseexcept() %s\n", (r?"fails":"succeeds"));
    return 0.0;
}
 
int main(void)
{
    some_computation();
    show_fe_exceptions();
 
    return 0;
}