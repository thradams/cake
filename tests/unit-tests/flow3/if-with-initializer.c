#pragma safety enable

/* if-with-initializer: the narrowing from the declared variable (or the explicit condition) reaches both branches of its scope */

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
        /* p narrowed non-null by the initializer acting as the condition */
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
    /* with an explicit condition `p == 0` the narrowing follows the condition: p null in the true branch, non-null in else */
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
    /* p is out of scope here */
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
