#pragma safety enable


struct X {
    int i;
};

void f(struct X* _Opt p)
{
    if (p && p->i || p->i > 2) //lint 33 pointer may be null
    {
    }
    else
    {
    }
}
