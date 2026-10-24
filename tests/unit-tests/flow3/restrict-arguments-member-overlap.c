#pragma safety enable

/* a restrict argument inside the object of another argument overlaps it */

struct Y { int v; };
struct X { struct Y y; int i; };

void put(struct Y* restrict dest, const struct X* src);

void f(struct X* x)
{
    put(&x->y, x); //lint 101 x->y is inside *x
}

void g(struct X* x, const struct X* other)
{
    put(&x->y, other); //lint 100 x and other may be the same object
}
