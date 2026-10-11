// a volatile object is never a constant, even when const qualified

const volatile int x = 3;
struct S { volatile int v; };
const struct S s = { 3 };
const int k = 3;

void f(void)
{
    int a[x];
    static_assert(sizeof a == 12); //lint 960

    int b[s.v];
    static_assert(sizeof b == 12); //lint 960

    int c[k];
    static_assert(sizeof c == 12); // OK: const without volatile is constant
}

int main() {
    return 0;
}
