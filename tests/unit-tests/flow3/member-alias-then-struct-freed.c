#pragma safety enable

_Uninitialized void* _Owner malloc(unsigned long n);
void free(void* _Owner _Nullable p);

struct X { int* _Owner p; };
struct Y { int i; };
struct Y* _Owner makey(void);

int f(void)
{
    struct X* _Owner x = malloc(sizeof(struct X));
    if (!x) return 0;
    x->p = malloc(sizeof(int));
    if (x->p)
    {
        *x->p = 1;
    }
    int* q = x->p;
    free(x->p);
    free(x);
    return q ? *q : 0; //lint 31 reported once
}

int g(void)
{
    struct Y* _Owner p = makey();
    if (!p) return 0;
    struct Y* q = p;
    free(p);
    return q->i; //lint 31 reported once
}

int h(int c)
{
    int* _Owner p = malloc(sizeof(int));
    if (!p) return 0;
    if (c)
    {
        *p = 1;
    }
    else
    {
        *p = 2;
    }
    free(p);
    return *p; //lint 31 written in both arms, then freed
}
