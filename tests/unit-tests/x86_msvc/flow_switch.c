/* Cake 0.12.26 x86_msvc */

void *make();
void free(void * p);

void f(int condition)
{
    void * p;

    p = make();
    /*switch*/
    {
        int __v0 = condition;
        if (__v0 == 1) goto __L1; /*case 1*/
        if (__v0 == 2) goto __L2; /*case 2*/
        goto __L3; /* default */

        {
            __L1: /*case 1*/ 
            {
                free(p);
            }
            goto __L0; /* break */

            __L2: /*case 2*/ 
            {
                free(p);
            }
            goto __L0; /* break */

            __L3: /* default */ 
            free(p);
            goto __L0; /* break */

        }
        __L0:;
    }
}


