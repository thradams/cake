//en.cppreference.com/w/c/numeric/math/nextafter.html
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <fenv.h>
 
int main(void)
{
    float from1 = 0, to1 = nextafterf(from1, 1);
    printf("The next representable float after %.2f is %.20g (%a)\n", from1, to1, to1);
 
    float from2 = 1, to2 = nextafterf(from2, 2);
    printf("The next representable float after %.2f is %.20f (%a)\n", from2, to2, to2);
 
    double from3 = nextafter(0.1, 0), to3 = 0.1;
    printf("The number 0.1 lies between two valid doubles:\n"
           "    %.56f (%a)\nand %.55f  (%a)\n", from3, from3, to3, to3);
 
    // difference between nextafter and nexttoward:
    long double dir = nextafterl(from1, 1); // first subnormal long double
    float x = nextafterf(from1, dir); // first converts dir to float, giving 0
    printf("Using nextafter, next float after %.2f (%a) is %.20g (%a)\n",
           from1, from1, x, x);
    x = nexttowardf(from1, dir);
    printf("Using nexttoward, next float after %.2f (%a) is %.20g (%a)\n",
           from1, from1, x, x);
 
    // special values
    {
        #pragma STDC FENV_ACCESS ON
        feclearexcept(FE_ALL_EXCEPT);
        double from4 = DBL_MAX, to4 = nextafter(from4, INFINITY);
        printf("The next representable double after %.2g (%a) is %.23f (%a)\n",
               from4, from4, to4, to4);
        if(fetestexcept(FE_OVERFLOW)) puts("   raised FE_OVERFLOW");
        if(fetestexcept(FE_INEXACT)) puts("   raised FE_INEXACT");
    } // end FENV_ACCESS block
 
    float from5 = 0.0, to5 = nextafter(from5, -0.0);
    printf("nextafter(+0.0, -0.0) gives %.2g (%a)\n", to5, to5);
}