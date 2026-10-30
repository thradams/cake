#define CHECK(p) if (p != 0 && p->x == 1) { int y = 1 / 0; }

struct S { int x; };

void f(const struct S* p)
{
    CHECK(p) //lint 37
}
