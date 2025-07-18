
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
        if (_R0 == 3) goto _CKL2; /*case 3*/
        goto _CKL0;

        {
            void * p2;
            void * p3;

            /*case 1*/ _CKL1:
            p2 = malloc(1);
            free(p2);
            /*break*/ goto _CKL0;

            /*case 3*/ _CKL2:
            p3 = malloc(1);
            free(p3);
            /*break*/ goto _CKL0;

        }
        _CKL0:;
    }
    free(p1);
}

