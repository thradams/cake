/* Cake 0.12.26 x86_msvc */
struct X {
    char * name;
};


void *calloc(unsigned int n, unsigned int size);

struct X *F(int i)
{
    struct X * p1;

    p1 = 0;
    /*switch*/
    {
        int __v0 = i;
        if (__v0 == 1) goto __L1; /*case 1*/
        if (__v0 == 2) goto __L3; /*case 2*/
        goto __L0;

        {
            struct X * p2;

            __L1: /*case 1*/ 
            p2 = calloc(1, 4);
            if (p2)
            {
                p1 = p2;
            }
            goto __L0; /* break */

            __L3: /*case 2*/ 
            goto __L0; /* break */

        }
        __L0:;
    }
    return p1;
}


