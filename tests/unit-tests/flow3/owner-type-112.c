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
    struct X x; // owner object (x.text) not moved (root see line 22); owner object (x.p1) not moved (root see line 22)
    x = f();
    compile_assert(x.text != 0);
    compile_assert(x.p1 != 0);
    //assert_state(x.i, "zero | not-zero");
    //assert_state(x.pY, "null | not-null");
    compile_assert(x.pY->p0 != 0);
    destroy(&x); // passing a possible uninitialized object  '(*&x).text' (see line 22); passing a possible uninitialized object  '(*&x).p1' (see line 22); passing a possible uninitialized object  '(*&x).i' (see line 22); passing a possible uninitialized object  '(*&x).pY' (see line 22)
}
