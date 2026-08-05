#pragma safety enable

/*
   Regression tests for the C2Y / C++-style if-with-initializer:

       if (T* p = expr) { ... }
       if (T* p = expr; condition) { ... } else { ... }

   The declared variable's scope is the whole if-statement (both
   branches), and the narrowing flow3 derives from testing it (or an
   explicit condition using it) must reach wherever that scope says it
   is visible: the true branch when the plain form is used, the false
   branch too when the initializer is combined with an explicit
   condition, else-if chains, etc.
*/

struct node
{
    struct node* _Opt next;
    int value;
};

struct node* _Opt get();
struct node* _Opt get_a();
struct node* _Opt get_b();
struct node* _Opt get_next(struct node* _Opt p);

void true_branch_narrowed(void)
{
    if (struct node* _Opt p = get())
    {
        /* p narrowed to non-null by the if-initializer itself acting
           as the condition -- safe to call p-> */
        int v = p->value;
        (void)v;
    }
}

void false_branch_is_null(void)
{
    if (struct node* _Opt p = get())
    {
        int v = p->value; // safe
        (void)v;
    }
    else
    {
        int v = p->value; //lint 33 -> operator applied to a null pointer
        (void)v;
    }
}

void explicit_condition_negated(void)
{
    /* if-with-initializer plus an explicit condition (p == 0): the
       narrowing must follow the CONDITION, not just "was p written
       to", so p is null in the true branch here and non-null in the
       false (else) branch -- the opposite of the plain form above. */
    if (struct node* _Opt p = get(); p == 0)
    {
        return;
    }
    else
    {
        int v = p->value; // safe -- this is the "p is non-null" branch
        (void)v;
    }
}

void early_return_idiom(void)
{
    if (struct node* _Opt p = get())
    {
        int v = p->value; // safe
        (void)v;
        return;
    }
    /* p is out of scope here (the if-statement's initializer scope
       ended), nothing to check */
}

void else_if_chain(void)
{
    if (struct node* _Opt p = get_a())
    {
        int v = p->value; // safe -- p narrowed by its own if
        (void)v;
    }
    else if (struct node* _Opt q = get_b())
    {
        int v = q->value; // safe -- q narrowed by its own else-if
        (void)v;
    }
}
