/* Cake x86_x64_gcc */
struct __tag5 {
    unsigned int short __control_word;
    unsigned int short __glibc_reserved1;
    unsigned int short __status_word;
    unsigned int short __glibc_reserved2;
    unsigned int short __tags;
    unsigned int short __glibc_reserved3;
    unsigned int __eip;
    unsigned int short __cs_selector;
    unsigned int __opcode;
    unsigned int __glibc_reserved4;
    unsigned int __data_offset;
    unsigned int short __data_selector;
    unsigned int short __glibc_reserved5;
    unsigned int __mxcsr;
};


extern int printf(char * __format, ...);
extern int fetestexcept(int __excepts);

void show_fe_exceptions(void)
{
    printf("current exceptions raised: ");
    if (fetestexcept(4))
    {
        printf(" FE_DIVBYZERO");
    }
    if (fetestexcept(32))
    {
        printf(" FE_INEXACT");
    }
    if (fetestexcept(1))
    {
        printf(" FE_INVALID");
    }
    if (fetestexcept(8))
    {
        printf(" FE_OVERFLOW");
    }
    if (fetestexcept(16))
    {
        printf(" FE_UNDERFLOW");
    }
    if (fetestexcept(61) == 0)
    {
        printf(" none");
    }
    printf("\n");
}


extern int feholdexcept(struct __tag5 * __envp);
extern int feclearexcept(int __excepts);
extern int feupdateenv(struct __tag5 * __envp);

double x2(double x)
{
    struct __tag5  curr_excepts;

    feholdexcept(&curr_excepts);
    printf("In x2():  x = %f\n", x = x * 2.0);
    show_fe_exceptions();
    feclearexcept(32);
    feupdateenv(&curr_excepts);
    return x;
}


extern int feraiseexcept(int __excepts);

int main(void)
{
    feclearexcept(61);
    feraiseexcept(1);
    show_fe_exceptions();
    printf("x2(DBL_MAX) = %f\n", x2(1.7976931348623157e+308));
    show_fe_exceptions();
    return 0;
}


