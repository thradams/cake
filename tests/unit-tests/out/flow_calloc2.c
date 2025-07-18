struct X {
    int i;
    void * p;
};


void *calloc(int i, int sz);
void free(void * p);

int main()
{
    struct X * p;

    p = calloc(1, 1);
    if (p)
    {
    }
    free(p);
}

