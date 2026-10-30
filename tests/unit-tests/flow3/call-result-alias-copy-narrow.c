#pragma safety enable

/* known bug: a copy of a member read through a call result that may be its argument holds a narrowed "!= 0" with no target, so a test of its member does not narrow (codegen.c:6204) */

struct L { int* s; };
struct S { struct L* head; };

struct S* complete(struct S* p);
void use_int(int* p);

/*
    entry flow of the call result:

    if (complete(s0) == s0) c = s0;
    else if (not null) c = new(); else c = 0;
*/

/* Must NOT warn: h is a copy of c->head, h->s was tested. */
void test_through_copy(struct S* s0)
{
    struct S* c = complete(s0);
    if (c)
    {
        struct L* h = c->head;
        if (h && h->s)
            use_int(h->s);
    }
}
