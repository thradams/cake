#pragma safety enable


struct Y {
    char* _Owner p0;
    int* _Owner p2;
    double i2;
};

struct X {
    char* _Owner text;
    int* _Owner p1;
    int i;
    struct Y* _Opt pY;
};

struct X f();
void destroy(_Dtor struct X *  x);

int main()
{
    struct X x;
    x = f();
    assert_state(x.text, "not-null ");
    assert_state(x.p1, "not-null ");
    assert_state(x.i, "zero | not-zero");
    assert_state(x.pY, "null | not-null");
    assert_state(x.pY->p0, "not-null ");    
    destroy(&x);
}
