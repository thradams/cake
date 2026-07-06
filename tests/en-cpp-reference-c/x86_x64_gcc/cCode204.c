/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);
extern int fegetround(void);

void show_fe_current_rounding_direction(void)
{
    printf("current rounding direction:  ");
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


extern double rint(double __x);
extern int fesetround(int __rounding_direction);

int main(void)
{
    int curr_direction;

    show_fe_current_rounding_direction();
    printf("+11.5 -> %+4.1f\n", rint(11.5));
    printf("+12.5 -> %+4.1f\n", rint(12.5));
    curr_direction = fegetround();
    fesetround(1024);
    show_fe_current_rounding_direction();
    printf("+11.5 -> %+4.1f\n", rint(11.5));
    printf("+12.5 -> %+4.1f\n", rint(12.5));
    fesetround(curr_direction);
    show_fe_current_rounding_direction();
    return 0;
}


