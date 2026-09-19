#pragma safety enable

/* after `if (p)` a dereference inside the true branch must not warn; the unguarded twin warns */

struct X
{
    struct X* _Opt next;
};

struct X* _Opt something();

void guarded(struct X* _Opt p)
{
    p = something();
    if (p)
    {
        /* p narrowed to non-null by the if -- no warning here */
        p = p->next;
    }
}

void unguarded(struct X* _Opt p)
{
    p = something();
    p = p->next; //lint 33 -> operator applied to a null pointer
}
