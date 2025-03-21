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
    static_state(x.text, "not-null ");
    static_state(x.p1, "not-null ");
    static_state(x.i, "zero | not-zero");
    static_state(x.pY, "null | not-null");
    static_state(x.pY->p0, "not-null ");    
    destroy(&x);
}
