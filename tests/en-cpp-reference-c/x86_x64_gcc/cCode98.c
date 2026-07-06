/* Cake x86_x64_gcc */

extern int printf(char * __format, ...);

int main(void)
{
    int i;

    i = 2;
    /*switch*/
    {
        int __v0 = i;
        if (__v0 == 1) goto __L1; /*case 1*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 3) goto __L3; /*case 3*/
        if (__v0 == 4) goto __L4; /*case 4*/
        if (__v0 == 5) goto __L5; /*case 5*/
        if (__v0 == 6) goto __L6; /*case 6*/
        goto __L0;

        {
            __L1: /*case 1*/ 
            printf("1");
            __L2: /*case 2*/ 
            printf("2");
            __L3: /*case 3*/ 
            printf("3");
            __L4: /*case 4*/ 
            __L5: /*case 5*/ 
            printf("45");
            goto __L0; /* break */

            __L6: /*case 6*/ 
            printf("6");
        }
        __L0:;
    }
    printf("\n");
    {
        int j;

        j = 0;
        for (; j < 2; j++)
        {
            int k;

            k = 0;
            for (; k < 5; k++)
            printf("%d%d ", j, k);
        }
    }
    printf("\n");
    {
        int j;

        j = 0;
        for (; j < 2; j++)
        {
            {
                int k;

                k = 0;
                for (; k < 5; k++)
                {
                    if (k == 2)
                    {
                        break;
                    }
                    printf("%d%d ", j, k);
                }
            }
        }
    }
}


