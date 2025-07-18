
void func(int e)
{
    /*switch*/
    {
        register int _R0 = e;
        if (_R0 == 0) goto _CKL1; /*case 0*/
        if (_R0 == 1) goto _CKL2; /*case 1*/
        goto _CKL0;

        {
            /*case 0*/ _CKL1:
            /*break*/ goto _CKL0;

            /*case 1*/ _CKL2:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
}

int main()
{
}

