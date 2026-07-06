/* Cake x64_msvc */

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
        int __a = n;
        if (__a == 1) goto __L1; /*case 1*/
        if (__a == 2) goto __L2; /*case 2*/
        if (__a == 3) goto __L3; /*case 3*/
        if (__a == 4) goto __L4; /*case 4*/
        if (__a == 5) goto __L6; /*case 5*/
        if (__a == 6) goto __L7; /*case 6*/
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
