//en.cppreference.com/w/c/numeric/math/isless.html
#include <stdio.h>
#include <math.h>
 
int main(void)
{
    printf("isless(2.0,1.0)      = %d\n", isless(2.0,1.0));
    printf("isless(1.0,2.0)      = %d\n", isless(1.0,2.0));
    printf("isless(INFINITY,1.0) = %d\n", isless(INFINITY,1.0));
    printf("isless(1.0,NAN)      = %d\n", isless(1.0,NAN));
 
    return 0;
}