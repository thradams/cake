/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    char c;
    unsigned long long ulong_num;
    long long long_num;

    c = 16;
    ulong_num = 291;
    printf("0x123 << 1  = %#llx\n0x123 << 63 = %#llx\n0x10  << 10 = %#x\n", ulong_num << 1, ulong_num << 63, c << 10);
    long_num = -1000;
    printf("-1000 >> 1 = %lld\n", long_num >> 1);
}


