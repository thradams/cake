/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

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
