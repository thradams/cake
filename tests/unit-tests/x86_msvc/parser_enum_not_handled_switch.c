/* Cake 0.12.26 x86_msvc */

void func(int e)
{
    /*switch*/
    {
        int __v0 = e;
        if (__v0 == 0) goto __L1; /*case 0*/
        if (__v0 == 1) goto __L2; /*case 1*/
        goto __L0;

        {
            __L1: /*case 0*/ 
            goto __L0; /* break */

            __L2: /*case 1*/ 
            goto __L0; /* break */

        }
        __L0:;
    }
}


int main()
{
}


