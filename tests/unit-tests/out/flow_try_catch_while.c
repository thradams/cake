struct X {
    char * name;
};


void *malloc(unsigned int size);

struct X *F(int i)
{
    struct X * p1;

    p1 = 0;
    /*try*/ if (1)
    {
        if (i == 1)
        {
            p1 = malloc(4U);
            while (0)
            {
            }
        }
        else
        {
            if (i == 3)
            {
                p1 = malloc(4U);
            }
        }
    }
    /*catch*/ else _CKL0:
    {
    }
    return p1;
}

