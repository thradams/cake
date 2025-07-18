struct X {
    char * name;
};


void *calloc(int n, int size);

void *f1()
{
    struct X * p;

    p = calloc(1, 4U);
    if (p)
    {
        p->name = calloc(1, 2);
    }
    return p;
}

