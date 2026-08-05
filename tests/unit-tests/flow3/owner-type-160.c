#pragma safety enable

struct X {
    char* _Owner text;
};

void x_destroy(_Dtor struct X* p);

int main()
{
    struct X x; // owner object (x.text) not moved (root see line 11)
    // warning: uninitialized object '&x.text'
    x_destroy(&x); //lint 30 passing a possible uninitialized object '(*&x).text' (see line 11)
}
