/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    printf("123 = %d\n", 123);
    printf("0123 = %d\n", 83);
    printf("0x123 = %d\n", 291);
    printf("12345678901234567890ull = %llu\n", 12345678901234567890ULL);
    printf("12345678901234567890u = %lu\n", 12345678901234567890L);
    printf("%llu\n", 9223372036854775808ULL);
    printf("%lld\n", -9223372036854775808LL);
}


