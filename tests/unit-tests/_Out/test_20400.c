#pragma safety enable

struct Z {
    int i;
};

struct Y {
    struct Z* /*_Opt*/ pZ;
};

struct X {
    struct Y* /*_Opt*/ pY;
};

void f(struct X* /*_Opt*/ p)
{
    if (p && p->pY && p->pY->pZ)
    {
        p->pY->pZ->i = 1;
    }
}
