#pragma safety enable


void* _Owner malloc(unsigned long size);
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

    static_state(x.p1, "null ");
    static_state(x.i, "zero");
    static_state(x.pY, "null ");

    destroy(&x);
}
