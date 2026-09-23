#pragma safety enable

struct S { int x; };

void arrow(struct S* p)
{
    p->x = 1;
    p->x += 10;
    compile_assert(p->x == 11);
}

void deref(int* p)
{
    *p = 1;
    *p += 10;
    compile_assert(*p == 11);
}

void through_local_pointer(void)
{
    int a = 1;
    int* p = &a;
    *p <<= 2;
    compile_assert(a == 4);
}

void correlated(struct S* p, int c)
{
    if (c) { p->x = 1; } else { p->x = 3; }
    p->x += 10;
    compile_assert(p->x == 11 || p->x == 13);
    compile_assert(p->x != 12);
}
