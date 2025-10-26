/* Cake 0.12.26 x86_msvc */

void g(void)
{
}


void h(void)
{
}


void i(void)
{
}


void f(int n)
{
    /*switch*/
    {
        int __v0 = n;
        if (__v0 == 1) goto __L1; /*case 1*/
        if (__v0 == 2) goto __L2; /*case 2*/
        if (__v0 == 3) goto __L3; /*case 3*/
        if (__v0 == 4) goto __L4; /*case 4*/
        if (__v0 == 5) goto __L6; /*case 5*/
        if (__v0 == 6) goto __L7; /*case 6*/
        goto __L0;

        {
            __L1: /*case 1*/ 
            __L2: /*case 2*/ 
            g();
            ;
            __L3: /*case 3*/ 
            h();
            __L4: /*case 4*/ 
            if (n < 3)
            {
                i();
                ;
            }
            else
            {
                return;
            }
            __L6: /*case 5*/ 
            while (0)
            {
                ;
            }
            __L7: /*case 6*/ 
            ;
        }
        __L0:;
    }
}


int main(void)
{
}


