/* Cake 0.12.26 x86_msvc */

void *calloc(int n, unsigned int size);

char *f(int i)
{
    char * p;

    p = calloc(1, 2);
    /*switch*/
    {
        int __v0 = i;
        if (__v0 == 1) goto __L1; /*case 1*/
        if (__v0 == 2) goto __L2; /*case 2*/
        goto __L0;

        {
            __L1: /*case 1*/ 
            goto __L0; /* break */

            __L2: /*case 2*/ 
            goto __L0; /* break */

        }
        __L0:;
    }
    return p;
}


