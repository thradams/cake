//en.cppreference.com/w/c/numeric/math/signbit.html
#include <math.h>
#include <stdio.h>
 
int main(void)
{
    printf("signbit(+0.0) = %d\n", signbit(+0.0));
    printf("signbit(-0.0) = %d\n", signbit(-0.0));
}