#pragma safety enable

/*
   Test for flow3's `case EXPR_UNARY_ASSERT:` handling.

   _Assert(expr) is equivalent to:

       if (!(expr)) exit(1);   // exit does not return

   So after the assert executes, only the branch where `expr` is TRUE is
   reachable -- the false branch is a dead end. flow3 therefore applies
   the true-branch refinements of `expr` to the current state and discards
   the false branch. In other words, everything the compiler could learn
   from `if (expr) { ...here... }` is learned unconditionally after the
   assert.

   Each function below pairs an asserted case (no warning expected) with,
   where useful, an un-asserted contrast that DOES warn -- so the file
   demonstrates both that the narrowing happens and that it was actually
   needed.
*/

#define NULL ((void*)0)

struct X
{
    struct X* _Opt next;
    int i;
};

struct X* _Opt get(void);

/* --------------------------------------------------------------------
   1. _Assert(p) narrows a nullable pointer to non-null.
   -------------------------------------------------------------------- */

void asserted(struct X* _Opt p)
{
    _Assert(p);
    p->i = 1; /* ok: assert proved p is non-null */
}

void not_asserted(struct X* _Opt p)
{
    p->i = 1; //lint 33 warning: pointer 'p' may be null
}

/* --------------------------------------------------------------------
   2. assert with an explicit comparison (_Assert(p != NULL)) narrows the
      same way -- the refinement comes from the true branch of the
      comparison, not from the bare pointer.
   -------------------------------------------------------------------- */

void asserted_cmp(struct X* _Opt p)
{
    _Assert(p != NULL);
    p->i = 1; /* ok */
}

/* --------------------------------------------------------------------
   3. assert on a struct member pointer narrows that member.
   -------------------------------------------------------------------- */

struct ctx
{
    struct X* _Opt p;
};

void asserted_member(struct ctx* c)
{
    _Assert(c->p);
    c->p->i = 1; /* ok */
}

/* --------------------------------------------------------------------
   4. A compound condition narrows every conjunct: after _Assert(p && p->next)
      both p and p->next are known non-null.
   -------------------------------------------------------------------- */

void asserted_and(struct X* _Opt p)
{
    _Assert(p && p->next);
    p->i = 1;       /* ok */
    p->next->i = 2; /* ok */
}

/* --------------------------------------------------------------------
   5. assert narrows the VALUE of an integer, not just null-ness. After
      the assert the fact holds for the constant analysis, so the
      compile_assert below must succeed (no diagnostic).
   -------------------------------------------------------------------- */

void asserted_value(int a)
{
    _Assert(a == 5);
    compile_assert(a == 5); /* holds because of the assert above */
}

void asserted_range(int a)
{
    _Assert(a > 0);
    // static_debug(a);
    compile_assert(a > 0); /* range refinement survives the assert */
}

/* --------------------------------------------------------------------
   6. The narrowing is state, not a permanent property: reassigning p to a
      maybe-null value after the assert drops the refinement, so the later
      dereference warns again. This confirms the assert only refines the
      current map -- it doesn't magically make the variable non-null
      forever.
   -------------------------------------------------------------------- */

void asserted_then_reassigned(struct X* _Opt p)
{
    _Assert(p);
    p = get();
    p->i = 1; //lint 33 warning: pointer 'p' may be null
}
