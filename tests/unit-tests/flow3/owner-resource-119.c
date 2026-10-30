#pragma safety enable


_Uninitialized void* _Nonnull _Owner malloc(unsigned long size);
void free(void* _Owner ptr);

struct Y {
    char* _Nonnull _Owner p0;
    int* _Nonnull _Owner p2;
    double i2;
};

struct X {
    char* _Owner text;
    int* _Owner p1;
    int i;
    struct Y* pY;
};

void f(const struct X* p);
void destroy(_Dtor struct X *  p);

int main()
{
    struct X x = {0};
    f(&x);

    compile_assert(x.p1 == 0);
    compile_assert(x.i == 0);
    compile_assert(x.pY == 0);

    destroy(&x);
}
