#pragma safety enable


struct X {
    char* _Owner p;
};
void x_destroy(_Dtor struct X* p);
void f(struct X* x)
{
    //source expression of _Dtor must be addressof
    [[cake::w19]]
    x_destroy(x);
}
