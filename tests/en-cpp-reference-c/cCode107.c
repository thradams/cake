//en.cppreference.com/w/c/language/floating_constant.html
#include <stdio.h>
 
int main(void)
{
    printf("15.0     = %a\n", 15.0);
    printf("0x1.ep+3 = %f\n", 0x1.ep+3);
 
    // Constants outside the range of type double.
    [[cake::w63]]
    printf("+2.0e+308 --> %g\n",  2.0e+308);
    [[cake::w63]]
    printf("+1.0e-324 --> %g\n",  1.0e-324);
    [[cake::w63]]
    printf("-1.0e-324 --> %g\n", -1.0e-324);
    [[cake::w63]]
    printf("-2.0e+308 --> %g\n", -2.0e+308);
}