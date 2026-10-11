/* A local function cannot use a named constant of the enclosing function */

int f(void)
{
    constexpr int K = 5;
    static const int* r1(void) { return &K; } //lint 1870
    static int r2(void) { return K; } //lint 1870
    (void)r1;
    return r2();
}
