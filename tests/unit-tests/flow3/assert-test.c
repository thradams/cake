#pragma safety enable

/* _Assert(expr) keeps only the true branch of expr, like `if (!(expr)) exit(1);` */

#define NULL ((void*)0)

struct X
{
    struct X* _Opt next;
    int i;
};

struct X* _Opt get(void);

/* 1. _Assert(p) narrows a nullable pointer to non-null */

void asserted(struct X* _Opt p)
{
    _Assert(p);
    p->i = 1; /* ok: assert proved p is non-null */
}

void not_asserted(struct X* _Opt p)
{
    p->i = 1; //lint 33 warning: pointer 'p' may be null
}

/* 2. _Assert(p != NULL) narrows the same way, from the comparison's true branch */

void asserted_cmp(struct X* _Opt p)
{
    _Assert(p != NULL);
    p->i = 1; /* ok */
}

/* 3. assert on a struct member pointer narrows that member */

struct ctx
{
    struct X* _Opt p;
};

void asserted_member(struct ctx* c)
{
    _Assert(c->p);
    c->p->i = 1; /* ok */
}

/* 4. _Assert(p && p->next) narrows every conjunct */

void asserted_and(struct X* _Opt p)
{
    _Assert(p && p->next);
    p->i = 1;       /* ok */
    p->next->i = 2; /* ok */
}

/* 5. assert narrows an integer's value, so the compile_assert below holds */

void asserted_value(int a)
{
    _Assert(a == 5);
    compile_assert(a == 5); /* holds because of the assert above */
}

void asserted_range(int a)
{
    _Assert(a > 0);
    compile_assert(a > 0); /* range refinement survives the assert */
}

/* 6. narrowing is state: reassigning p to a maybe-null value drops it, the dereference warns again */

void asserted_then_reassigned(struct X* _Opt p)
{
    _Assert(p);
    p = get();
    p->i = 1; //lint 33 warning: pointer 'p' may be null
}
