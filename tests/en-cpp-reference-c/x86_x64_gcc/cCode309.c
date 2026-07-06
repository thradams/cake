/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int atoi(char * __nptr);

int main(void)
{
    printf("%i\n", atoi(" -123junk"));
    printf("%i\n", atoi(" +321dust"));
    printf("%i\n", atoi("0"));
    printf("%i\n", atoi("0042"));
    printf("%i\n", atoi("0x2A"));
    printf("%i\n", atoi("junk"));
    printf("%i\n", atoi("2147483648"));
}


