/* Cake x86_x64_gcc */
struct S {
    int x;
};


extern int printf(char * __format, ...);

int main(void)
{
    double d;
    struct S  s;

    d = 3.1400000000000001;
    printf("The double %.2f (%a) is: ", d, d);
    {
        unsigned int long n;

        n = 0;
        for (; n < 8L; ++n)
        printf("0x%02x ", ((unsigned char *)&d)[n]);
    }
    (void)s;
}


