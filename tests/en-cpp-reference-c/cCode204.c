//en.cppreference.com/w/c/numeric/fenv/feround.html
#include <stdio.h>
#include <math.h>
#include <fenv.h>
 
#pragma STDC FENV_ACCESS ON
void show_fe_current_rounding_direction(void)
{
    printf("current rounding direction:  ");
    switch (fegetround()) {
           case FE_TONEAREST:  printf ("FE_TONEAREST");  break;
           case FE_DOWNWARD:   printf ("FE_DOWNWARD");   break;
           case FE_UPWARD:     printf ("FE_UPWARD");     break;
           case FE_TOWARDZERO: printf ("FE_TOWARDZERO"); break;
           default:            printf ("unknown");
    };
    printf("\n");
}
 
int main(void)
{
    /* Default rounding direction */
    show_fe_current_rounding_direction();
    printf("+11.5 -> %+4.1f\n", rint(+11.5)); /* midway between two integers */
    printf("+12.5 -> %+4.1f\n", rint(+12.5)); /* midway between two integers */
 
    /* Save current rounding direction. */
    int curr_direction = fegetround();
 
    /* Temporarily change current rounding direction. */
    fesetround(FE_DOWNWARD);
    show_fe_current_rounding_direction();
    printf("+11.5 -> %+4.1f\n", rint(+11.5));
    printf("+12.5 -> %+4.1f\n", rint(+12.5));
 
    /* Restore default rounding direction. */
    fesetround(curr_direction);
    show_fe_current_rounding_direction(); 
 
    return 0;
}