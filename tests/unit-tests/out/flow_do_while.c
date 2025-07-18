
void *malloc(unsigned int size);
void free(void * ptr);

int main()
{
    void * p;

    p = malloc(1);
    do
    {
        free(p);
    }
    while (0);
}

