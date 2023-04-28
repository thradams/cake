struct X { int i; };
void f(struct X* p) {}
int main() {
    const struct X* p;
    f(p);
}