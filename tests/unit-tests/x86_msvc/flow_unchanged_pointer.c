/* Cake 0.12.26 x86_msvc */
struct Y;

struct X {
    struct Y * pY;
};

struct Y {
    int i;
};


void *calloc(int n, unsigned long size);
void f(struct Y * p);
void free(void * ptr);

int main()
{
    struct X * p;

    p = calloc(1, 4);
    if (p)
    {
        p->pY = calloc(1, 4);
        if (p->pY)
        {
            f(p->pY);
            p->pY->i = 1;
        }
        free(p->pY);
        free(p);
    }
}


