//en.cppreference.com/w/c/language/real_floating_types.html
#include <stdio.h>
#include <float.h>
 
int main(void)
{
    /* The value being converted can be represented exactly in the new type. */
    /* The value is unchanged in the new type.                               */
    double d = 1.0;
    float f = d;
    printf("%f\n", f);
 
    /* The value being converted is in the range of values that can be represented */
    /* in the new type but cannot be represented exactly.                          */
    /* implementation defined                                                      */
    d = 1.23456789012345;
    printf("%.14f\n", d);
    f = d;
    printf("%.14f\n", f);
 
    /* The value being converted is outside the range of values that can be */
    /* represented in the type.                                             */
    /* undefined behavior                                                   */
    d = DBL_MAX;
    f = d;
    printf("%f\n", f);
 
    return 0;
}