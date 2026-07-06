/* Cake 0.14.05 x86_msvc */

void f(int e)
{
    /*switch*/
    {
        int __a = e;
        if (__a == 0) goto __L1; /*case 0*/
        if (__a == 1) goto __L2; /*case 1*/
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
