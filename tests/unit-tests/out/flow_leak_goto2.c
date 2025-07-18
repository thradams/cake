
void *malloc(int size);
void free(void * ptr);

void f(int condition)
{
    int * p;

    p = malloc(4U);
    if (condition)
    {
        goto end;
    }
    free(p);
    end:
}

