#pragma safety enable

struct X {
    char* _Owner text;
};

void x_destroy(_Dtor struct X* p);

int main()
{
    struct X x;
    // warning: uninitialized object '&x.text'
    [[cake::w30]]
    x_destroy(&x);
}
