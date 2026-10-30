#pragma safety enable



struct X {
    char * _Owner text;
};

void f(int condition) {
    struct X x1 = {};
    struct X x2 = {};
    struct X * p = condition ? &x1 : &x2;

}
