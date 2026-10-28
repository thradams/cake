#pragma flow enable

/* the caller's restrict check needs only the flow: a pointer parameter always points to an object, with or without nullable */

struct X { int i; };


void copy(struct X* restrict dest, const struct X* src);

void same_pointer(struct X* a)
{
    copy(a, a); //lint 101
}

void same_value(struct X* a)
{
    struct X* q = a;
    copy(a, q); //lint 101
}

void same_address(void)
{
    struct X x = { 0 };
    copy(&x, &x); //lint 101
}

/* a and b may be the same object (a == b): warns as with safety */
void different(struct X* a, struct X* b)
{
    copy(a, b); //lint 100
}

void different_locals(void)
{
    struct X x = { 0 };
    struct X y = { 0 };
    copy(&x, &y);
}

struct S { struct X* p; };
void same_member_pointer(struct S* s)
{
    copy(s->p, s->p); //lint 101
}
