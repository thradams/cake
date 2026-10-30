#pragma safety enable

// one report per argument, not one per loop path.

struct X { int* _Owner p; };

void destroy(_Dtor struct X* x);

void f(int n)
{
    struct X x;
    while (n--)
        x.p = 0;

    destroy(&x); //lint 30
}
