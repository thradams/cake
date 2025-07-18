
void *make();
void free(void * p);

void f(int condition)
{
    void * p;

    p = make();
    /*switch*/
    {
        register int _R0 = condition;
        if (_R0 == 1) goto _CKL1; /*case 1*/
        if (_R0 == 2) goto _CKL2; /*case 2*/
        goto /*default*/ _CKL3;

        {
            /*case 1*/ _CKL1:
            {
                free(p);
            }
            /*break*/ goto _CKL0;

            /*case 2*/ _CKL2:
            {
                free(p);
            }
            /*break*/ goto _CKL0;

            /*default*/ _CKL3:
            free(p);
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
}

