//en.cppreference.com/w/c/language/cast.html
#include <stdio.h>
 
int main(void)
{
    // examining object representation is a legitimate use of cast
    double d = 3.14;
    printf("The double %.2f (%a) is: ", d, d);
    for (size_t n = 0; n < sizeof d; ++n)
        printf("0x%02x ", ((unsigned char*)&d)[n]);
 
    // edge cases
    struct S { int x; } s;
//    (struct S)s; // error; not a scalar type
                   // even though casting to the same type does nothing
    (void)s; // okay to cast any type to void
}