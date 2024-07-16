//en.cppreference.com/w/c/string/byte/atoi.html
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    printf("%i\n", atoi(" -123junk"));
    printf("%i\n", atoi(" +321dust"));
    printf("%i\n", atoi("0"));
    printf("%i\n", atoi("0042")); // treated as a decimal number with leading zeros
    printf("%i\n", atoi("0x2A")); // only leading zero is converted discarding "x2A"
    printf("%i\n", atoi("junk")); // no conversion can be performed
    printf("%i\n", atoi("2147483648")); // UB: out of range of int
}