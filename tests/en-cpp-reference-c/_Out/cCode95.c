//en.cppreference.com/w/c/language/bit_field.html
#include <stdio.h>
 
struct S
{
    // will usually occupy 8 bytes:
    // 5 bits: value of b1
    // 27 bits: unused
    // 6 bits: value of b2
    // 15 bits: value of b3
    // 11 bits: unused
    unsigned b1 : 5;
    unsigned :0; // start a new unsigned int
    unsigned b2 : 6;
    unsigned b3 : 15;
};
 
int main(void)
{
    printf("%zu\n", sizeof(struct S)); // usually prints 8
}