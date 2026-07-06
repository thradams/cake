/* Cake x86_x64_gcc */

extern double nan(char * __tagb);
extern void *memcpy(void * __dest, void * __src, unsigned int long __n);
extern int printf(char * __format, ...);

int main(void)
{
    double f1;
    unsigned int long f1n;
    double f2;
    unsigned int long f2n;
    double f3;
    unsigned int long f3n;

    f1 = nan("1");
    memcpy(&f1n, &f1, 8L);
    printf("nan(\"1\")   = %f (%lx)\n", f1, f1n);
    f2 = nan("2");
    memcpy(&f2n, &f2, 8L);
    printf("nan(\"2\")   = %f (%lx)\n", f2, f2n);
    f3 = nan("0xF");
    memcpy(&f3n, &f3, 8L);
    printf("nan(\"0xF\") = %f (%lx)\n", f3, f3n);
}


