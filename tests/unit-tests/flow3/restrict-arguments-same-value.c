#pragma safety enable

/* with the flow the restrict check also sees the same object through another pointer; the same expression is reported once, by the parser */

struct X { int i; };

void copy(struct X* restrict dest, const struct X* src);

void same_pointer(struct X* a)
{
    copy(a, a); //lint 101
}

struct S { struct X* _Nonnull p; };
void same_member_pointer(struct S* s)
{
    copy(s->p, s->p); //lint 101
}

void same_value(struct X* a)
{
    struct X* q = a;
    copy(a, q); //lint 101
}
