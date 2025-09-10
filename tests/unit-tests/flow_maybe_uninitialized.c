#pragma safety enable

int printf(const char* restrict format, ...);

struct X {
    int i;
};

//warning: uninitialized object 'x.i'
[[cake::w30]]
void f(_Ctor struct X* x, int i)
{
    if (i > 0)
    {
        x->i = 1;
    }
}

//external variables, in this case object *x cannot leave scope
//with uninitialized values.

int main()
{
    struct X x;
    f(&x, 1);
    printf("%d", x.i);
}
