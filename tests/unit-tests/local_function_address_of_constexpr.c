/* N3884: the address of a named constant of the enclosing function cannot be taken */

int f(void)
{
    constexpr int K = 5;
    static const int* r1(void) { return &K; } //lint 1220
    static int r2(void) { return K; }
    (void)r1;
    return r2();
}
