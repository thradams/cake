/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

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
