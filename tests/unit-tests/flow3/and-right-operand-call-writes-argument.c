#pragma safety enable

struct X { int i; };
_Bool get(struct X* _Nullable* pp);

int f(int* _Nullable e)
{
    struct X* p = 0;
    const _Bool ok = e != 0 && get(&p);
    if (!ok)
    {
        return 2;
    }
    /* get may have set p */
    if (p != 0)
    {
        return 1;
    }
    return 0;
}

int g(int* _Nullable e)
{
    struct X* p = 0;
    if (e != 0 && get(&p))
    {
        if (p != 0)
        {
            return 1;
        }
    }
    return 0;
}
