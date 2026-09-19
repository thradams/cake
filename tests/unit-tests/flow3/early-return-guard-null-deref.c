#pragma safety enable

/* backwards guard `if (p != NULL) return; p->i = 1;` is a guaranteed null dereference, also inside loops and for members */

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

void backwards_guard_member(const struct ctx* c)
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

/* contrast: the correct guard with `p == NULL` (a cast of 0 to pointer) is clean */
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
