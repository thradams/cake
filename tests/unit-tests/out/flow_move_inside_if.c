struct X {
    char * name;
};


void *calloc(unsigned int n, unsigned int size);

struct X *F(int i)
{
    struct X * p1;
    struct X * p2;

    p1 = 0;
    p2 = calloc(1, 4U);
    if (p2)
    {
        p1 = p2;
    }
    return p1;
}

