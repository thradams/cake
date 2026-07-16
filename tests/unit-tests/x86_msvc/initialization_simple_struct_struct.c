/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:29 */

struct Y {
    double d;
};

struct X {
    struct Y y;
    int i;
};

void T3()
{
    struct X x;

    x.y.d = 0;
    x.i = 1;
}
