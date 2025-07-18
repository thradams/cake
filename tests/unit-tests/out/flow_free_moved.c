
void *calloc(unsigned long n, unsigned long size);
void free(void * ptr);

int main()
{
    int * p;
    int * p2;

    p = calloc(1, 4U);
    p2 = p;
    free(p2);
    free(p);
}

