
void *calloc(int n, int i);

char *f(int i)
{
    char * p;

    p = calloc(1, 2);
    /*switch*/
    {
        register int _R0 = i;
        if (_R0 == 1) goto _CKL1; /*case 1*/
        goto _CKL0;

        {
            /*case 1*/ _CKL1:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    return p;
}

