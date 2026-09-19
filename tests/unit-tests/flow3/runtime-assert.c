#pragma safety enable

/* _Assert is a keyword that always narrows (even with assert stripped), is usable as an expression, and lowers to a runtime check only with -runtime-asserts */

#define NULL ((void*)0)
#define assert(x) ((void)0)          /* release: standard assert does nothing */

struct X { struct X* _Opt next; int i; };

/* Explicit comparison. */
void compare(struct X* _Opt p)
{
    _Assert(p != NULL);
    p->i = 1;                        /* ok */
}

/* Conjunction narrows every conjunct. */
void conjunction(struct X* _Opt p)
{
    _Assert(p && p->next);
    p->i = 1;                        /* ok */
    p->next->i = 2;                  /* ok */
}

/* Value range, with a message (compile_assert grammar). */
void range(int n)
{
    _Assert(n > 0, "n must be positive");
    compile_assert(n > 0);           /* provable from the _Assert */
}

/* expression position: _Assert inside a comma expression, the narrowing persists afterward */
int in_expression(struct X* _Opt p)
{
    (_Assert(p != NULL), (void)0);
    return p->i;                     /* ok: p was narrowed non-null */
}
