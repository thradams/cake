//en.cppreference.com/w/c/language/operator_arithmetic.html
#include <stdio.h>
enum {ONE=1, TWO=2};
int main(void)
{
    char c = 0x10;
    unsigned long long ulong_num = 0x123;
    printf("0x123 << 1  = %#llx\n"
           "0x123 << 63 = %#llx\n"   // overflow truncates high bits for unsigned numbers
           "0x10  << 10 = %#x\n",    // char is promoted to int
           ulong_num << 1, ulong_num << 63, c << 10);
    long long long_num = -1000;
    printf("-1000 >> 1 = %lld\n", long_num >> ONE);  // implementation defined
}