#pragma safety enable

int printf(const char* restrict format, ...);

struct X {
    int i;
};

//warning: uninitialized object 'x.i'

void f(_Ctor struct X* x, int i)
{
    if (i > 0)
    {
        x->i = 1;
    }
} //lint 30

//external variables, in this case object *x cannot leave scope
//with uninitialized values.

int main()
{
    struct X x;
    f(&x, 1);
    printf("%d", x.i);
}
