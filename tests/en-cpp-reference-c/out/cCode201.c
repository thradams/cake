//en.cppreference.com/w/c/numeric/fenv/feexceptflag.html
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
    printf("\n");
}
 
int main(void)
{
    fexcept_t excepts;
 
    /* Setup a "current" set of exception flags. */
    feraiseexcept(FE_INVALID);
    show_fe_exceptions();
 
    /* Save current exception flags. */
    fegetexceptflag(&excepts,FE_ALL_EXCEPT);
 
    /* Temporarily raise two other exceptions. */
    feclearexcept(FE_ALL_EXCEPT);
    feraiseexcept(FE_OVERFLOW | FE_INEXACT);
    show_fe_exceptions();
 
    /* Restore previous exception flags. */
    fesetexceptflag(&excepts,FE_ALL_EXCEPT);
    show_fe_exceptions();
 
    return 0;
}