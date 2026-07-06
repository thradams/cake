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


extern int fegetround(void);

void show_fe_rounding_method(void)
{
    printf("current rounding method:    ");
    /*switch*/
    {
        int __v0 = fegetround();
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 1024) goto __L2; /*case 1024*/
        if (__v0 == 2048) goto __L3; /*case 2048*/
        if (__v0 == 3072) goto __L4; /*case 3072*/
        goto __L5; /* default */

        {
            __L1: /*case 0*/ 
            printf("FE_TONEAREST");
            goto __L0; /* break */

            __L2: /*case 1024*/ 
            printf("FE_DOWNWARD");
            goto __L0; /* break */

            __L3: /*case 2048*/ 
            printf("FE_UPWARD");
            goto __L0; /* break */

            __L4: /*case 3072*/ 
            printf("FE_TOWARDZERO");
            goto __L0; /* break */

            __L5: /* default */ 
            printf("unknown");
        }
        __L0:;
    }
    ;
    printf("\n");
}


void show_fe_environment(void)
{
    show_fe_exceptions();
    show_fe_rounding_method();
}


extern int fesetround(int __rounding_direction);
extern int feraiseexcept(int __excepts);
extern int fesetenv(struct __tag5 * __envp);

int main()
{
    printf("On startup:\n");
    show_fe_environment();
    fesetround(1024);
    feraiseexcept(1);
    printf("\nBefore restoration:\n");
    show_fe_environment();
    fesetenv(((struct __tag5 *)-1));
    printf("\nAfter restoring default environment:\n");
    show_fe_environment();
}


