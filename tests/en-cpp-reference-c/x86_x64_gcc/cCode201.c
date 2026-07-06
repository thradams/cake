/* Cake x86_x64_gcc */

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


extern int feraiseexcept(int __excepts);
extern int fegetexceptflag(unsigned int short * __flagp, int __excepts);
extern int feclearexcept(int __excepts);
extern int fesetexceptflag(unsigned int short * __flagp, int __excepts);

int main(void)
{
    unsigned int short excepts;

    feraiseexcept(1);
    show_fe_exceptions();
    fegetexceptflag(&excepts, 61);
    feclearexcept(61);
    feraiseexcept(40);
    show_fe_exceptions();
    fesetexceptflag(&excepts, 61);
    show_fe_exceptions();
    return 0;
}


