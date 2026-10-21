#pragma safety enable

/* flow.md 6.3: a call changes only the pointee of its argument; what the pointee's pointer
   members point to keeps its state (through const, nothing reachable changes) */

struct Y { int * _Opt p; };
struct X { struct Y * q; };

void f_const(const struct X * p);
void f_mut(struct X * p);
void g_const(const struct Y * p);

void through_const(int * i)
{
    struct Y y = { .p = i };
    struct X x = { .q = &y };
    f_const(&x);
    *y.p = 1;            /* through const: y kept */
}

void through_mut(int * i)
{
    struct Y y = { .p = i };
    struct X x = { .q = &y };
    f_mut(&x);
    *y.p = 1;            /* only x may have changed: y kept */
}

void direct_const(int * i)
{
    struct Y y = { .p = i };
    g_const(&y);
    *y.p = 1;            /* y itself is the const pointee: kept */
}
