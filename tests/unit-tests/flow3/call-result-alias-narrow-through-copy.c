#pragma safety enable

/* the result of a call may be its argument: a member read through it has two targets, and a test of it still narrows (codegen.c register_struct_types_and_functions) */

struct L { int* _Opt s; };
struct S { struct L* _Opt head; };

struct S* _Opt complete(struct S* p);
void use(struct L* p);
void use_int(int* p);

/*
    entry flow of the call result:

    if (complete(s0) == s0) c = s0;
    else if (not null) c = new(); else c = 0;
*/

/* Must NOT warn: c->head was tested, through both targets. */
void test_then_cast(struct S* s0)
{
    struct S* _Opt c = complete(s0);
    if (c && c->head)
        use((struct L*)c->head);
}

/* must warn: h->s is not tested */
void not_tested(struct S* s0)
{
    struct S* _Opt c = complete(s0);
    if (c)
    {
        struct L* _Opt h = c->head;
        if (h)
            use_int(h->s); //lint 35 'h->s' may be null
    }
}
