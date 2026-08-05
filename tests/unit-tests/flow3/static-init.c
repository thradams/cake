#pragma flow enable

constexpr int a = 1;
constexpr int b = 2;
int c;

void f()
{
    compile_assert(a == 1);
    compile_assert(b == 2);
    c = 3;
    compile_assert(c == 3);

    c = 4;
    compile_assert(c == 4);
}
