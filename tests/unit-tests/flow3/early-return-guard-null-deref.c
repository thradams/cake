#pragma safety enable

/*
   Requested check: an early-return guard with the condition backwards.

     if (p != NULL)
         return;
     p->i = 1;

   The `if` only returns when p IS non-null, so the only way execution
   reaches `p->i = 1` is when the condition was false, i.e. p == NULL at
   that point. It's a guaranteed (not just "possible") null dereference.

   Verified this fires correctly -- not just for this minimal shape, but
   across several more "complex function" variations: a non-void function
   returning a value, a member pointer (ctx->p), and the same guard nested
   inside a for-loop and a while-loop body. All correctly warn with
   "-> operator applied to a null pointer" at the dereference.
*/

#define NULL ((void*)0)

struct X
{
    int i;
};

void backwards_guard(struct X* _Opt p)
{
    if (p != NULL)
        return;
    p->i = 1; //lint 33 -> operator applied to a null pointer
}

int backwards_guard_return_value(struct X* _Opt p)
{
    if (p != NULL)
        return 1;
    p->i = 1; //lint 33 -> operator applied to a null pointer
    return 0;
}

struct ctx
{
    struct X* _Opt p;
};

void backwards_guard_member(struct ctx* c)
{
    if (c->p != NULL)
        return;
    c->p->i = 1; //lint 33 -> operator applied to a null pointer
}

void backwards_guard_in_for(struct X* _Opt p, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (p != NULL)
            return;
        p->i = 1; //lint 33 -> operator applied to a null pointer
    }
}

void backwards_guard_in_while(struct X* _Opt p)
{
    while (1)
    {
        if (p != NULL)
            return;
        p->i = 1; //lint 33 -> operator applied to a null pointer
        break;
    }
}

/* Contrast: the correct (non-backwards) guard is safe and warns about
   nothing -- included so the file demonstrates both sides.

   This specifically exercises `p == NULL` (not `p == 0` or `!p`): with
   NULL defined as `((void*)0)` (the common, standard-conforming
   definition), the comparison goes through an EXPR_CAST node for the
   integer constant 0 being cast to a pointer type. flow3 used to give up
   on any integer-to-pointer cast (treating it as an unknowable address)
   instead of recognizing that specifically casting the constant 0 to a
   pointer type is the null pointer constant -- so this exact guard used
   to produce a false positive on the next line, while `p == 0` and `!p`
   (which don't go through EXPR_CAST) worked correctly. Fixed in the
   EXPR_CAST integer-to-pointer handling in flow3.c. */
void correct_guard(struct X* _Opt p)
{
    if (p == NULL)
        return;
    p->i = 1; /* ok: p is non-null here */
}

void correct_guard_block_form(struct X* _Opt p)
{
    if (p != NULL)
    {
        p->i = 1; /* ok: narrowed non-null inside the true branch */
    }
}

void unguarded_after_empty_if(struct X* _Opt p)
{
    if (p != NULL)
    {
        /* nothing here -- doesn't narrow anything for what follows */
    }
    p->i = 1; //lint 33 -> operator applied to a null pointer
}
