#pragma safety enable

struct X { int i; };

void copy(struct X* restrict dest, const struct X* src);

void f(struct X* a, const struct X* b)
{
    copy(a, a); //lint 101 same pointer
    copy(a, b); //lint 100 a and b are parameters that may be the same object
}

void g(struct X* a)
{
    struct X* q = a;
    copy(a, q); //lint 101 same object through another pointer
}

void h(void)
{
    struct X x = { 0 };
    struct X y = { 0 };
    copy(&x, &x); //lint 101
    copy(&x, &y);
}
