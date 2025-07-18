
void *malloc(int size);
void free(void * ptr);

void f(int c)
{
    int * p;

    p = malloc(4U);
    if (c)
    {
        free(p);
    }
}

