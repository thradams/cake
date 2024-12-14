#pragma safety enable



struct X {
    char * _Owner _Opt text;
};

void f(int condition) {
    struct X x1 = {};
    struct X x2 = {};
    struct X * p = condition ? &x1 : &x2;

    static_debug(p);
}
