#pragma safety enable


struct X {
    int i;
};

void f(struct X* _Opt p)
{
    //pointer may be null
    [[cake::w33]]
    if (p && p->i || p->i > 2)
    {
    }
    else
    {
    }
}
