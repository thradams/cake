#pragma safety enable

/* KNOWN GAP: `p = f(p)` in a try whose catch releases p reports use-after-end once per throw path, the ENDED fact of the consumed object reaches the catch merge (expressions.c unary_expression) */

#define NULL ((void*)0)

struct E { int x; };

struct E* _Owner _Opt tail(struct E* _Owner p);
struct E* _Owner _Opt make(void);
void del(struct E* _Owner _Opt p);
int maybe(void);

/* the expressions.c shape: several throws reach one catch, one after the self-consuming reassignment */
struct E* _Owner _Opt consume_in_try(void)
{
    struct E* _Owner _Opt p = make();
    try
    {
        if (p == NULL)
            throw;

        if (maybe())
            throw; /* reaches the catch with p still live */

        p = tail(p); /* consumes the old value, result back into p */

        if (p == NULL)
            throw; /* reaches the catch with p == NULL */
    }
    catch
    {
        /* must not warn: on the consuming arm p is the call's result (NULL when it threw), elsewhere p is live */
        del(p);
        p = NULL;
    }
    return p;
}

/* same bug through goto: the accumulated join becomes the live map */
void consume_then_goto(void)
{
    struct E* _Owner _Opt p = make();
    if (p == NULL)
        return;

    if (maybe())
        goto cleanup;

    p = tail(p);

    if (p == NULL)
        goto cleanup;

cleanup:
    del(p); //lint 31 object '(*p)' lifetime has ended -- same join-accumulation bug as the try above
}

/* control 1: break is clean, its join is merged with replace semantics at loop exit */
void consume_then_break_ok(void)
{
    struct E* _Owner _Opt p = make();
    if (p == NULL)
        return;

    while (1) 
    {
        if (maybe())
            break;

        p = tail(p);

        if (p == NULL)
            break;

        break;
    }

    del(p);
}

/* control 2: no try/catch, no join, clean */
struct E* _Owner _Opt consume_no_try_ok(void)
{
    struct E* _Owner _Opt p = make();
    if (p == NULL)
        return NULL;

    p = tail(p);

    return p;
}
