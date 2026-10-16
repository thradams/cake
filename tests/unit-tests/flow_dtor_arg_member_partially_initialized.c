#pragma safety enable

// a member initialized on only one path is still possibly uninitialized when
// the struct is passed by pointer.

struct X { int* _Owner _Opt p; };

void destroy(_Dtor struct X * x);

void f(int c)
{
    struct X x;
    if (c)
        x.p = 0;

    destroy(&x); //lint 30
}
