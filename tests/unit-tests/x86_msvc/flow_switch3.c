/* Cake 0.14.05 x86_msvc */

void *calloc(int n, int i);

char *f(int i)
{
    char * p;

    p = calloc(1, 2);
    /*switch*/
    {
        int __a = i;
        if (__a == 1) goto __L1; /*case 1*/
        goto __L0;

        {
            __L1: /*case 1*/ 
            goto __L0; /* break */

        }
        __L0:;
    }
    return p;
}
