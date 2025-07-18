struct X {
    int i;
};


void f(struct X * p)
{
    int i;
    int i3;
    int i2;

    i = p ? p->i : 0;
    i3 = p->i;
    i2 = p ? 0 : p->i;
}

