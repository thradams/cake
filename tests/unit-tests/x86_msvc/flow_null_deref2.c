/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct X {
    int i;
};

void f(struct X * p)
{
    int i;
    int i3;
    int i2;

    i = p ? p->i : 0;
    i3 = p->i;
    i2 = p ? 0 : p->i;
}
