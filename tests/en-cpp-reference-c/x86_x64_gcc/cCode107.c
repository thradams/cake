/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    printf("15.0     = %a\n", 15.0);
    printf("0x1.ep+3 = %f\n", 15.0);
    printf("+2.0e+308 --> %g\n", .7976931348623157E+308);
    printf("+1.0e-324 --> %g\n", 0.0);
    printf("-1.0e-324 --> %g\n", -0.0);
    printf("-2.0e+308 --> %g\n", .7976931348623157E+308);
}


