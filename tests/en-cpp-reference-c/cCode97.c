//en.cppreference.com/w/c/language/boolean_type.html
#include <stdio.h>
 
int main(void)
{
    _Bool a = 0.0;

    _Bool b = 8;

    _Bool c = 0.0/0.0;


    printf("%d\n", a);
    printf("%d\n", b);
    printf("%d\n", c);
 
    return 0;
}