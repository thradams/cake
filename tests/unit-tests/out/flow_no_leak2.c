
void *malloc(int size);
void free(void * ptr);

int main()
{
    int * p;

    p = malloc(4U);
    if (p != 0)
    {
        free(p);
    }
}

