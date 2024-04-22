#pragma nullable enable
#pragma ownership enable


struct X {
    char * _Owner text;
};

int main() {
    struct X x1 = {};
    struct X x2 = {};
    struct X * p = condition ? &x1 : &x2;

    static_debub(p);
}
