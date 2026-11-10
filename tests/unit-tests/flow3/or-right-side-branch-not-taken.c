#pragma safety enable

struct s { int x; };
void d(_Dtor struct s* p);

void f(struct s* _Nullable p, int c)
{
    if (c)
    {
        d(p);
        p = 0;
    }
    _Assert(p == 0 || p->x == 0); /* no warning 30 */
}

void g(struct s* _Nullable p, int c)
{
    if (c)
    {
        d(p);
        p = 0;
    }
    if (p && p->x == 0) /* no warning 30 */
    {
    }
}
