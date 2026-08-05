#pragma safety enable

/*
   `p = f(p);` where f CONSUMES the owner and returns a new one.

   This is valid and must not warn. The call transfers ownership of the old
   value into f, f returns a (possibly different) owner, and the assignment
   puts that back into p. At no point does anyone touch the consumed value:
   the argument is read before the call, the result is written after it.

   Real instance: expressions.c's

       p_expression_node = postfix_expression_tail(ctx, p_expression_node, is_discarded);

   where postfix_expression_tail takes `struct expression* _Owner` and returns
   `struct expression* _Owner _Opt`. Restructuring the call site into two
   variables does NOT change flow3's verdict (tried: the diagnostic just moves
   to the new line), so the pattern is worth pinning as a sample in its own
   right rather than being worked around in the caller.

   See also owner-consumed-then-nulled-in-try.c for the harder variant, where
   a `throw` between the call and the end of the block merges the consumed
   state into a catch that then frees the pointer.
*/

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

/*
   Repeated application -- a parse loop shape: each round consumes what the
   previous round produced.

   This was the case that exposed the bug the other two forms hid. After the
   loop, p carries two alternatives: the pre-loop object (the zero-iteration
   arm, which never reached `tail`) and the object produced by the last
   iteration. Consuming the pre-loop object inside the body marked it ENDED,
   and the aggregate state check read that fact without asking which branch
   recorded it -- so the arm that never ran the body was reported as a
   use-after-end. Fixed by giving flow3_object_leaves_in_state the same
   origin filter the per-alternative loop already used.
*/
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
