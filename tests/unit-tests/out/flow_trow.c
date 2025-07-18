
int f(int condition)
{
    int * p;

    p = 0;
    /*try*/ if (1)
    {
        int * p2;

        p2 = p;
        if (condition)
        {
            /*throw*/ goto _CKL0;
        }
        p = 0;
    }
    /*catch*/ else _CKL0:
    {
    }
}

