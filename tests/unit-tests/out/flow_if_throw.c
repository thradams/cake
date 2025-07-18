
int f();

int main()
{
    int i;

    /*try*/ if (1)
    {
        if (f())
        {
            i = 1;
            /*throw*/ goto _CKL0;
        }
        i = 0;
    }
    /*catch*/ else _CKL0:
    {
    }
}

