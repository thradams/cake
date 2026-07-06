/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int fetestexcept(int __excepts);
extern int feclearexcept(int __excepts);

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
    feclearexcept(61);
    printf("\n");
}


extern int feraiseexcept(int __excepts);

double some_computation(void)
{
    int r;

    r = feraiseexcept(40);
    printf("feraiseexcept() %s\n", (r ? "fails" : "succeeds"));
    return 0.0;
}


int main(void)
{
    some_computation();
    show_fe_exceptions();
    return 0;
}


