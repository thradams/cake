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
        if (__v0 == 3) goto __L2; /*case 3*/
        goto __L0;

        {
            void * p2;
            void * p3;

            __L1: /*case 1*/ 
            p2 = malloc(1);
            free(p2);
            goto __L0; /* break */

            __L2: /*case 3*/ 
            p3 = malloc(1);
            free(p3);
            goto __L0; /* break */

        }
        __L0:;
    }
    free(p1);
}


