/* Cake 0.13.29 x86_msvc */

int main()
{
    int i;

    /*switch*/
    {
        int __a = i;
        if (__a == 1) goto __L1; /*case 1*/
        if (__a == 2) goto __L2; /*case 2*/
        if (__a == 1) goto __L3; /*case 1*/
        goto __L0;

        {
            __L1: /*case 1*/ 
            goto __L0; /* break */

            __L2: /*case 2*/ 
            goto __L0; /* break */

            __L3: /*case 1*/ 
            goto __L0; /* break */

        }
        __L0:;
    }
}
