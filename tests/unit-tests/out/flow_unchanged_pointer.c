struct Y {
    int i;
};

struct X {
    struct Y * pY;
};


void *calloc(int n, unsigned long size);
void f(struct Y * p);
void free(void * ptr);

int main()
{
    struct X * p;

    p = calloc(1, 4U);
    if (p)
    {
        p->pY = calloc(1, 4U);
        if (p->pY)
        {
            f(p->pY);
            p->pY->i = 1;
        }
        free(p->pY);
        free(p);
    }
}

