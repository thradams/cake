/* Cake x86_msvc */
struct Y {
    int i;
};

struct X {
    struct Y  y;
};


void *malloc(int size);

struct X *create(struct Y * pY)
{
    struct X * p;

    p = malloc(4);
    if (p)
    {
        p->y = *pY;
    }
    return p;
}


