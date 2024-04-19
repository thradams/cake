struct X { int i; };
struct X f() {
    struct X x = {};
    return x;
}
int main() {
    f().i = 1;
}

