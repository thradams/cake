/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int puts(char * __s);

int main(void)
{
    double result;

    result = .7976931348623157E+308;
    printf("1.0/0.0 == %f\n", .7976931348623157E+308);
    if (.7976931348623157E+308 == (__builtin_huge_val()))
    {
        puts("1.0/0.0 == HUGE_VAL");
    }
}


