/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

union U {
    int b;
    double c;
};

struct X {
    double a;
    union U u;
    int d;
};

int main()
{
    struct X x;

    x.a = 1;
    x.u.b = 2;
    x.d = 3;
}
