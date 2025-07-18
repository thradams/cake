
void *malloc(unsigned int size);
void free(void * ptr);

void f(int i)
{
    void * p1;

    p1 = malloc(1);
    /*switch*/
    {
        register int _R0 = i;
        if (_R0 == 1) goto _CKL1; /*case 1*/
        if (_R0 == 2) goto _CKL2; /*case 2*/
        goto _CKL0;

        {
            /*case 1*/ _CKL1:
            {
                void * p2;

                p2 = malloc(1);
                free(p2);
            }
            /*break*/ goto _CKL0;

            /*case 2*/ _CKL2:
            {
                void * p3;

                p3 = malloc(1);
                free(p3);
            }
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    free(p1);
}

