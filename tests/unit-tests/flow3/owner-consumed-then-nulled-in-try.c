#pragma safety enable

/* `p = f(p)` consuming p, with a throw before and after the call: the catch's free(p) must not report use-after-end, the consumed fact belongs only to the arm that ran the call (expressions.c compound literal) */

#define NULL ((void*)0)

struct E { int x; };

struct E* _Owner _Opt tail(struct E* _Owner p);
struct E* _Owner _Opt make(void);
void del(struct E* _Owner _Opt p);
int cond(void);

struct E* _Owner _Opt consume_then_throw(void)
{
    struct E* _Owner _Opt p = NULL;
    try
    {
        p = make();
        if (p == NULL)
            throw;

        /* Unrelated failure while p is still live: the catch must free it. */
        if (cond())
            throw;

        p = tail(p);
        if (p == NULL)
            throw;
    }
    catch
    {
        /* p is the untouched object or null, never the consumed one: must not warn */
        del(p);
        p = NULL;
    }

    return p;
}

/* control: the same shape without the intervening throw */
struct E* _Owner _Opt consume_no_intervening_throw(void)
{
    struct E* _Owner _Opt p = NULL;
    try
    {
        p = make();
        if (p == NULL)
            throw;

        p = tail(p);
        if (p == NULL)
            throw;
    }
    catch
    {
        del(p); /* ok */
        p = NULL;
    }

    return p;
}
