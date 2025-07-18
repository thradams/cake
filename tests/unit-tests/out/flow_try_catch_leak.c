
void *malloc(int i);
int rand();
void free(void * p);

int main()
{
    char * s;

    s = malloc(1);
    /*try*/ if (1)
    {
        if (rand())
        {
            free(s);
        }
        else
        {
            /*throw*/ goto _CKL0;
        }
    }
    /*catch*/ else _CKL0:
    {
    }
}

