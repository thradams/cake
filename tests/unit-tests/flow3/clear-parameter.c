#pragma safety enable

/* _Clear parameter: the call site sees the pointee as all zero, and the implementation must leave every member 0 at every exit; const _Clear (1930) and _Clear on non-pointers (1940) are rejected */

struct inner
{
    int a;
    int b;
};

struct outer
{
    struct inner in;
    int c;
};

void clear_outer(_Clear struct outer* p)
{
    p->in.a = 0;
    p->in.b = 0;
    p->c = 0;
}

void clear_outer_forgets_a_member(_Clear struct outer* p)
{
    p->in.a = 0;
    p->in.b = 0;
    /* forgot p->c = 0; */
} //lint 69 _Clear parameter 'p' pointee (.c) is never set to zero

void clear_outer_does_nothing(_Clear struct outer* p)
{
    /* does not touch *p at all -- flagged for every leaf member */
} //lint 69 69 69 _Clear parameter 'p' pointee (.in.a) is never set to zero; _Clear parameter 'p' pointee

void whole_struct_cleared(void)
{
    struct outer o = { { 1, 2 }, 3 };
    clear_outer(&o);
    /* expected: every member, including the nested struct's, is 0 */
    compile_assert(o.in.a == 0);
    compile_assert(o.in.b == 0);
    compile_assert(o.c == 0);
}

void without_the_call_values_are_unchanged(void)
{
    /* contrast: no _Clear call here, so the original values survive */
    struct outer o = { { 1, 2 }, 3 };
    compile_assert(o.in.a == 1);
    compile_assert(o.in.b == 2);
    compile_assert(o.c == 3);
}

/* uncomment to see error 1930 (const _Clear pointee) and error 1940 (_Clear on a non-pointer) */
