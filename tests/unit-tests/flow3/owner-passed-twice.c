#pragma safety enable

struct X { int i; };

void take2(struct X* _Owner a, struct X* _Owner b);
void take_and_view(struct X* _Owner a, struct X* b);

void f(struct X* _Owner p)
{
    take2(p, p); //lint 32 the first argument moved p
}

void g(struct X* _Owner p)
{
    /* valid: take_and_view is checked for a and b being the same object (warning 100) */
    take_and_view(p, p);
}
