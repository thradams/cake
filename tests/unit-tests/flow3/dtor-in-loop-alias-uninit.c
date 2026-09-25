#pragma safety enable

struct E { int i; struct E* _Owner _Opt left; };
struct E* _Owner _Clear make(void);
void d(_Dtor struct E* _Owner p);
int f(void);

struct E* _Owner g(struct E* _Owner p)
{
    while (f())
    {
        struct E* _Owner n = make();
        if (f())
        {
            d(n);
            break;
        }
        n->left = p;
        p = n;
    }

    /* FALSE POSITIVE, to be fixed: p->i is uninitialized only after d(n), where p does not point to n */
    if (p->i) {} //lint 30
    return p;
}
