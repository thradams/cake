
int main()
{
    int i;

    /*switch*/
    {
        register int _R0 = i;
        if (_R0 == 1) goto _CKL1; /*case 1*/
        if (_R0 == 2) goto _CKL2; /*case 2*/
        if (_R0 == 1) goto _CKL3; /*case 1*/
        goto _CKL0;

        {
            /*case 1*/ _CKL1:
            /*break*/ goto _CKL0;

            /*case 2*/ _CKL2:
            /*break*/ goto _CKL0;

            /*case 1*/ _CKL3:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
}

