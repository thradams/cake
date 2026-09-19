#pragma safety enable

/* two distinct non-null pointers may alias: `if (p == q)` is not decidable, neither branch is dead (list->head == list->tail) */

#define NULL ((void*)0)

struct T { int x; };
struct list { struct T* _Opt head; struct T* _Opt tail; };

/* The single-element-list test: reachable, not dead. */
void one_element(struct list* p)
{
    if (p->head == NULL)
        return;
    if (p->head == p->tail)
    {
        p->head->x = 1;          /* reachable */
    }
}

/* Two non-null parameters: both arms of the equality are live. */
void two_params(struct T* _Opt p, struct T* _Opt q)
{
    if (p && q)
    {
        if (p == q) { p->x = 1; }
        else        { q->x = 2; }
    }
}

/* Still decidable: comparison with NULL narrows as before. */
void with_null(struct T* _Opt p)
{
    if (p == NULL)
        return;
    p->x = 1;                    /* p proven non-null */
}
