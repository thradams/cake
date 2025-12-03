/* Cake x86_msvc */

void *malloc(unsigned int size);
void free(void * ptr);

void f(int i)
{
    void * p1;

    p1 = malloc(1);
    /*switch*/
    {
        int __v0 = i;
        if (__v0 == 1) goto __L1; /*case 1*/
        if (__v0 == 2) goto __L2; /*case 2*/
        goto __L0;

        {
            __L1: /*case 1*/ 
            {
                void * p2;

                p2 = malloc(1);
                free(p2);
            }
            goto __L0; /* break */

            __L2: /*case 2*/ 
            {
                void * p3;

                p3 = malloc(1);
                free(p3);
            }
            goto __L0; /* break */

        }
        __L0:;
    }
    free(p1);
}


