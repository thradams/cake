#pragma safety enable

struct X {
    int i;
};

void f(struct X* _Opt p)
{
    int i = p ? p->i : 0; //no warning

    int i3 = p->i;      //lint 33 warning: pointer may be null

    int i2 = p ? 0 : p->i; //lint 33 warning: pointer may be null
}

