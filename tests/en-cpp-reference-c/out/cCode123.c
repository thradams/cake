//en.cppreference.com/w/c/language/operator_arithmetic.html
#include <stdio.h>
#include <stdint.h>
 
int main(void)
{
    uint32_t a = 0x12345678;
    uint16_t mask = 0x00f0;
 
    printf("Promoted mask:\t%#010x\n"
           "Value:\t\t%#x\n"
           "Setting bits:\t%#x\n"
           "Clearing bits:\t%#x\n"
           "Selecting bits:\t%#010x\n"
           , mask
           , a
           , a | mask
           , a & ~mask
           , a & mask
    );
}