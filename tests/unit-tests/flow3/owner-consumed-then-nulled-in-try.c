#pragma safety enable

/*
   The harder variant of self-consuming-reassignment.c: `p = f(p);` where f
   consumes the owner, with a throw between the call and the end of the try
   block, so the consumed state merges into a catch that then frees p.

   Neither path reaching the catch has an ended object to free:

     - throwing BEFORE the call (the `cond()` arm below): tail never ran, so
       p still holds the object it was given. Releasing it is correct, and
       is the whole reason the catch frees p at all.
     - throwing AFTER the call: tail consumed the old value and returned
       null, p was overwritten with that null, and the release is a no-op.

   The consumed-at-line-N fact belongs only to the arm that actually ran the
   call, and on that arm p no longer refers to the consumed object. Merging
   it into the catch without asking which arm recorded it reports the free
   as a use-after-end.

   Real instance, cake's own postfix_expression_compound_func_literal in
   expressions.c:

       p_expression_node->last_token = p_previous_token;
       p_expression_node = postfix_expression_tail(ctx, p_expression_node, is_discarded);
       if (p_expression_node == NULL)
           throw;
     }
     catch
     {
       expression_delete(p_expression_node);   // warning 31 here
       p_expression_node = NULL;
     }

   As with the plain form, restructuring the call site into two variables
   does not change the verdict -- the diagnostic just moves to the new line
   -- so this is pinned as a sample rather than worked around in the caller.

   self-consuming-reassignment.c notes that its own loop case was fixed by
   giving flow3_object_leaves_in_state the origin filter the per-alternative
   loop already used; this is the same question asked at a catch join.
*/

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
        /*
           KNOWN FALSE POSITIVE. p is either the untouched object or null,
           never the consumed one, so this must not warn. The //lint marker
           records today's wrong answer so the suite stays green; an
           unmatched marker fails the suite, so fixing the analyser will
           fail this test and prompt deleting the marker -- and the control
           case below guards against "fixing" it by going silent everywhere.
        */
        del(p); //lint 31 object lifetime has ended
        p = NULL;
    }

    return p;
}

/*
   Control: the same shape without the intervening throw already passes.
   Kept next to the failing case so a fix can be checked against both.
*/
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
