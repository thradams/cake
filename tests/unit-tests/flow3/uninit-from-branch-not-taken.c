#pragma safety enable

struct s { int x; };
void d(_Dtor struct s* p);
void use(int i);

void f(struct s* _Nullable p, int c)
{
    if (c)
    {
        d(p);
        p = 0;
    }
    if (p)
    {
        use(p->x); /* no warning: p->x is uninitialized only where p is null */
    }
}
