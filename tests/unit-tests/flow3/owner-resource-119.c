#pragma safety enable


void* _Owner _Uninitialized malloc(unsigned long size);
void free(void* _Owner ptr);

struct Y {
    char* _Owner p0;
    int* _Owner p2;
    double i2;
};

struct X {
    char* _Owner _Opt text;
    int* _Owner _Opt p1;
    int i;
    struct Y* _Opt pY;
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
