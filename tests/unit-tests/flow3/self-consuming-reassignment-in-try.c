#pragma safety enable

/*
   `p = f(p);` inside a try, where a later throw reaches a catch that releases p.

   KNOWN GAP -- and the residue of a bug that is mostly fixed. See
   self-consuming-reassignment.c for the plain and loop forms, both of which are
   clean: the call consumes the old value, the result lands back in p, and
   nothing touches the consumed object.

   Real instance: expressions.c:2752/2758, cake's own unary_expression.

       try
       {
           ...
           p_expression_node = postfix_expression_tail(ctx, p_expression_node, is_discarded);
           if (p_expression_node == NULL)
               throw;
       }
       catch
       {
           expression_delete(p_expression_node);   // warning 31: lifetime has ended
           p_expression_node = NULL;
       }

   That site reported 66 warnings before the origin filter was extended to the
   aggregate state check (flow3_object_leaves_in_state); it reports 3 now --
   one per throw path that reaches the catch.

   Why the remaining case is harder. A catch is reached from MANY throw sites at
   once. Some arms hold p live (the throws before the call); one arm consumed the
   old object and holds the call's result, which is NULL exactly when that arm
   threw -- so `del(p)` there is a no-op. The ENDED fact belongs to the consumed
   object, which p no longer names, but the merge brings it along and the arms
   are not separable by origin the way a loop's back-edge arms were.

   Same family as the retained-alias gap in
   owner-moved-into-container-keeps-view.c: an ENDED fact recorded against an
   object that a name no longer refers to.
*/

#define NULL ((void*)0)

struct E { int x; };

struct E* _Owner _Opt tail(struct E* _Owner p);
struct E* _Owner _Opt make(void);
void del(struct E* _Owner _Opt p);
int maybe(void);

/* The expressions.c shape: several throws reach one catch, and one of them is
   after the self-consuming reassignment. */
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

        static_debug(p);
        if (p == NULL)
            throw; /* reaches the catch with p == NULL */
    }
    catch
    {
        /* Must not warn: on the arm that consumed the old object, p holds the
           call's result, and that result is NULL precisely when this arm threw,
           so this is a no-op. On every other arm p is live and this is the
           intended release. */
        del(p); //lint 31 object '(*p)' lifetime has ended -- consumed object reaches the catch merge
        p = NULL;
    }
    return p;
}

/*
   Same bug through GOTO, which shares the mechanism: both goto and throw
   accumulate into a join map (flow3_map_accumulate_into_join) that then BECOMES
   the live map, so the appended union survives.
*/
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

/*
   Control 1: BREAK is clean, and that is the discriminating case. break uses the
   same accumulate_into_join, but its join is folded in later by
   flow3_map_merge_arms at loop exit -- REPLACE semantics -- whereas goto and
   throw let the accumulated join become the live map directly.
*/
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

/*
   Control 2: the same reassignment with no try/catch around it. Clean -- there
   is no join at all to carry the consumed object's state anywhere.
*/
struct E* _Owner _Opt consume_no_try_ok(void)
{
    struct E* _Owner _Opt p = make();
    if (p == NULL)
        return NULL;

    p = tail(p);

    return p;
}
