#pragma safety enable

struct Z {
    int i;
};

struct Y {
    struct Z* pZ;
};

struct X {
    struct Y* pY;
};

void f(struct X* _Opt p)
{
    if (p && p->pY && p->pY->pZ)
    {
        p->pY->pZ->i = 1;
    }
}
