/* the note suggests 'static' only for constexpr or const objects with constant value (a, b) */

void f(int n)
{
    constexpr int a = 1;
    const int b = 2;
    const int c = n * 2;
    int d = 3;
    static int local() { return a + b + c + d; } //lint 1870 1870 1870 1870
    local();
}
