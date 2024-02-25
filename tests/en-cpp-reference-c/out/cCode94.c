//en.cppreference.com/w/c/language/bit_field.html
#include <stdio.h>
 
struct S
{
    // will usually occupy 4 bytes:
    // 5 bits: value of b1
    // 11 bits: unused
    // 6 bits: value of b2
    // 2 bits: value of b3
    // 8 bits: unused
    unsigned b1 : 5, : 11, b2 : 6, b3 : 2;
};
 
int main(void)
{
    printf("%zu\n",sizeof(struct S)); // usually prints 4
}