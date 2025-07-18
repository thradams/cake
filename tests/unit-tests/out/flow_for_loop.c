
void *malloc(unsigned long size);
void free(void * ptr);

int main()
{
    void * p;

    p = 0;
    {
        int i;
        i = 0;
        for (; i < 2; i++)
        {
            p = malloc(1);
        }
    }
    free(p);
}

