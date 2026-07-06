/* Cake x86_msvc */
struct X {
    int i;
};


struct X * g;
void f(struct X * p)
{
    g = p;
}


void *calloc(int n, unsigned long size);
void free(void * ptr);

int main()
{
    struct X * p;

    p = calloc(1, 4);
    if (p)
    {
        f(p);
        free(p);
    }
    if (g)
    {
        g->i = 1;
    }
}


