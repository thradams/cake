#pragma safety enable

void free(void* _Owner _Opt p);

struct X { int i; };

void f(struct X* _Owner p, struct X* p2)
{
    free(p);
    p2->i = 1; //lint 100 p2 is freed when it is p
}

void g(struct X* _Owner p, struct X* restrict p2)
{
    free(p);
    p2->i = 1;
}

void guarded(struct X* _Owner p, struct X* p2)
{
    if (p == p2)
    {
        free(p);
        return;
    }
    free(p);
    p2->i = 1;
}

void h(struct X* p, struct X* p2)
{
    p->i = 1;
    p2->i = 2;
}
