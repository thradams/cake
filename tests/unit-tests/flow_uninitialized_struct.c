#pragma safety enable

struct X {
    char* _Owner text;
};

void x_destroy(_Dtor struct X* p);

int main()
{
    struct X x;
    // warning: uninitialized object '&x.text'
    x_destroy(&x); //lint 30
}
