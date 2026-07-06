/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    signed int long n;

    printf("%zu\n", 8L);
    printf("%s\n", "ld");
    printf("%+ld\n", -9223372036854775808L);
    printf("%+ld\n", 9223372036854775807L);
    n = 7;
    printf("%+ld\n", n);
}


