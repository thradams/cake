#pragma safety enable

/* a null pointer constant (0, nullptr) assigned to a pointer is a null pointer, like (void*)0: `->` on it is a possible null dereference */

struct X { int i; };

void zero(int a)
{
    struct X x = { 0 };
    struct X* _Opt q = &x;
    if (a) q = 0;
    q->i = 1; //lint 33
}

void null_pointer_constant(int a)
{
    struct X x = { 0 };
    struct X* _Opt q = &x;
    if (a) q = nullptr;
    int k = q->i; //lint 33
    (void)k;
}

void cast(int a)
{
    struct X x = { 0 };
    struct X* _Opt q = &x;
    if (a) q = (void*)0;
    q->i = 1; //lint 33
}

/* the null comes from a branch that cannot be taken here */
void same_condition(int a)
{
    struct X x = { 0 };
    struct X* _Opt q = 0;
    if (a) q = &x;
    if (a) q->i = 1;
}
