/* Cake 0.12.26 x86_msvc */
struct X {
    char * name;
};


void *calloc(int n, int size);

void *f1()
{
    struct X * p;

    p = calloc(1, 4);
    if (p)
    {
        p->name = calloc(1, 2);
    }
    return p;
}


