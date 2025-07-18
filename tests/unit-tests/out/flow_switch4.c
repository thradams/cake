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
        register int _R0 = i;
        if (_R0 == 1) goto _CKL1; /*case 1*/
        if (_R0 == 2) goto _CKL3; /*case 2*/
        goto _CKL0;

        {
            struct X * p2;

            /*case 1*/ _CKL1:
            p2 = calloc(1, 4U);
            if (p2)
            {
                p1 = p2;
            }
            /*break*/ goto _CKL0;

            /*case 2*/ _CKL3:
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    return p1;
}

