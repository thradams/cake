#pragma safety enable

/* `p = f(p)` where f consumes the owner and returns a new one is valid and must not warn (expressions.c postfix_expression_tail) */

#define NULL ((void*)0)

struct E { int x; };

struct E* _Owner _Opt tail(struct E* _Owner p);
struct E* _Owner _Opt make(void);
void del(struct E* _Owner _Opt p);

/* The plain form: consume and reassign, no branching in between. */
void self_consume_ok(void)
{
    struct E* _Owner _Opt p = make();
    if (p == NULL)
        return;

    p = tail(p); /* ok: the old value is moved into tail, the new one lands in p */

    del(p);
}

/* Same, with the result checked before use. */
void self_consume_checked_ok(void)
{
    struct E* _Owner _Opt p = make();
    if (p == NULL)
        return;

    p = tail(p);
    if (p == NULL)
        return; /* tail consumed the old value and returned nothing to release */

    del(p);
}

/* parse-loop shape: the zero-iteration arm must not be reported as use-after-end of the object the body consumed (origin filter in flow3_object_leaves_in_state) */
void self_consume_loop_ok(int n)
{
    struct E* _Owner _Opt p = make();
    if (p == NULL)
        return;

    while (n-- > 0)
    {
        p = tail(p);
        if (p == NULL)
            return;
    }

    del(p);
}
