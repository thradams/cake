#pragma safety enable


struct X {
    char* _Owner p;
};
void x_destroy(_Dtor struct X* p);
void f(struct X* x)
{

    x_destroy(x); //lint 19 source expression of _Dtor must be addressof
}
