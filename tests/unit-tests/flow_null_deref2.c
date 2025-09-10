#pragma safety enable

struct X {
    int i;
};

void f(struct X* _Opt p)
{
    int i = p ? p->i : 0; //no warning

    //warning: pointer may be null
    [[cake::w33]]
    int i3 = p->i;

    //warning: pointer may be null
    [[cake::w33]]
    int i2 = p ? 0 : p->i; //warning
}

