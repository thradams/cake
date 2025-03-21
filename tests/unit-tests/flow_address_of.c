#pragma safety enable


struct X {
    char* _Owner p;
};
void x_destroy(_Dtor struct X*  p);
void f(struct X* x)
{
    x_destroy(x);
}

#pragma cake diagnostic check "-Wmust-use-address-of"
