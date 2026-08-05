#pragma safety enable

/*
   Two distinct non-null pointers may ALIAS -- comparing them for equality is
   not decidable, so neither branch of `if (p == q)` is dead.

   flow3 models each non-null _Opt pointer as its own synthesized pointee, so it
   used to treat `p == q` (different pointees) as always-false and mark the
   then-branch unreachable. That is wrong: nothing proves the two pointers don't
   point at the same object. e.g. `if (list->head == list->tail)` after a
   head != NULL guard is the ordinary single-element-list test.

   Only these pointer comparisons stay decidable: NULL vs non-null (not equal),
   both NULL (equal), and a pointer compared with itself (equal).
*/

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
