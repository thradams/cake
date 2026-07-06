/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    double d;
    float f;

    d = 1.0;
    f = d;
    printf("%f\n", f);
    d = 1.23456789012345;
    printf("%.14f\n", d);
    f = d;
    printf("%.14f\n", f);
    d = 1.7976931348623157e+308;
    f = d;
    printf("%f\n", f);
    return 0;
}


