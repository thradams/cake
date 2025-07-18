
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
        register int _R0 = n;
        if (_R0 == 1) goto _CKL1; /*case 1*/
        if (_R0 == 2) goto _CKL2; /*case 2*/
        if (_R0 == 3) goto _CKL3; /*case 3*/
        if (_R0 == 4) goto _CKL4; /*case 4*/
        if (_R0 == 5) goto _CKL6; /*case 5*/
        if (_R0 == 6) goto _CKL7; /*case 6*/
        goto _CKL0;

        {
            /*case 1*/ _CKL1:
            /*case 2*/ _CKL2:
            g();
            ;
            /*case 3*/ _CKL3:
            h();
            /*case 4*/ _CKL4:
            if (n < 3)
            {
                i();
                ;
            }
            else
            {
                return;
            }
            /*case 5*/ _CKL6:
            while (0)
            {
                ;
            }
            /*case 6*/ _CKL7:
            ;
        }
        _CKL0:;
    }
}

int main(void)
{
}

