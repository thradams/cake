//en.cppreference.com/w/c/numeric/math/islessequal.html
#include <stdio.h>
#include <math.h>
 
int main(void)
{
    printf("islessequal(2.0,1.0)      = %d\n", islessequal(2.0,1.0));
    printf("islessequal(1.0,2.0)      = %d\n", islessequal(1.0,2.0));
    printf("islessequal(1.0,1.0)      = %d\n", islessequal(1.0,1.0));
    printf("islessequal(INFINITY,1.0) = %d\n", islessequal(INFINITY,1.0));
    printf("islessequal(1.0,NAN)      = %d\n", islessequal(1.0,NAN));
 
    return 0;
}