struct X { int i; };
struct X f() {
    struct X x = {0};
    return x;
}
int main() {
    f().i = 1;
}
#pragma cake diagnostic check "-E122"

