/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct Y;

struct X {
    struct Y * pY;
};

struct Z;

struct Y {
    struct Z * pZ;
};

struct Z {
    int i;
};

void f(struct X * p)
{
    if (p && p->pY && p->pY->pZ)
    {
        p->pY->pZ->i = 1;
    }
}
