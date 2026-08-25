#pragma safety enable

/*
   False "object '*e' lifetime has ended" after the pointer has been
   reassigned.

       e = tail(e);

   moves the old pointee into the callee (its lifetime really does end) and
   assigns the returned owner back to e. After that statement e points at
   something else -- a fresh object, or NULL -- so the ended object is no
   longer reachable through e and `e_delete(e)` in the catch is safe.

   flow3 keeps the ENDED fact attached to e and reports it at the catch.

   The trigger is the member write through e before the move
   (`e->last_token = pt;`). Without that line the pointee is not tracked as
   its own object and there is no false positive -- remove it and this file
   goes clean.

   Reduced from src/expressions.c postfix_expression / unary_expression,
   both of which do exactly this:

       p_expression_node->last_token = p_previous_token;
       p_expression_node = postfix_expression_tail(ctx, p_expression_node, ...);
       ...
       catch { expression_delete(p_expression_node); }

   Mechanism (two wrong theories ruled out first):

     NOT "the assignment fails to clear the destination". It does clear --
     flow3_check_assigment reaches flow3_check_object_init_assigment with
     INIT_OBJ, which clears the destination's alternatives (flow3.c ~5387).

     NOT "the ended object is no longer the one the pointer references".
     It still is: the member write above makes flow3 synthesize ONE pointee
     object for e, and that same synthesized object is still referenced after
     the reassignment. A "same pointer?" test at the report site answers yes
     here and would not suppress this.

   What actually survives is the synthesized pointee's STATE. The move marks
   that object ENDED; reassigning e leaves it referencing the same synthesized
   object, which still carries ENDED even though it now models a different
   runtime object.

   UPDATE -- theory 2 above is wrong, and the mechanism is narrower than
   "the pointee's state survives". Measured with static_debug either side of
   the reassignment:

       before:  e == 'uALRAm:'                       (the make_a pointee)
       after:   e ==  (null)  |  e == 'uALRI0:'      (a NEW object)

   so `e` does NOT still reference the ended object after `e = tail(e);`.
   The report comes from the CATCH, and it needs BOTH throws to appear:

     - replace the pre-move `if (pt == 0) throw;` with a return -> clean
     - drop the post-move `if (e == 0) throw;`        -> clean

   i.e. the catch merges a path where e names the old pointee and it is alive
   (the pt == 0 throw, before the move) with a path where that pointee ended
   (the move), and the report pairs the first path's pointer value with the
   second path's lifetime fact.

   flow3 already has the machinery meant to prevent exactly that: the
   p_origin_filter argument of flow_check_object_access, whose own comment
   says the caller should pass "that alternative's ->origin" when descending
   into a pointee. The REF recursion now does pass it (it used to forward the
   caller's filter unchanged), but the report still fires: the ENDED
   alternative's origin passes flow_map_is_ancestor_or_self against the
   pointer alternative's origin. The merge re-tags alternatives' origins, so
   by the time the catch sees them the two paths are no longer distinguishable
   that way -- the same union-over-predecessors correlation problem the rest of
   this folder's history is about, surfacing here through the throw-join
   instead of a loop.

   FIXED, together with catch-merges-path-before-owner-move.c: each throw now
   contributes its state to the catch under its own snapshot map as the
   alternatives' origin, so the "consumed at line N" fact stays paired with
   the arm that ran the call instead of applying to every arm.

   The fix must not weaken the genuine cases:
   ../param-owner-may-leave-object-consumed.c, case
   plain_param_consumes_on_one_path, is a real bug that an over-broad
   suppression silences (tried, reverted).
*/

struct T { int line; };
struct E { struct T* last_token; int x; };

void e_delete(struct E* _Owner _Opt p);
struct E* _Owner _Opt tail(struct E* _Owner p);
struct E* _Owner _Opt make_a(void);
struct T* _Opt prev_token(void);

struct E* _Owner _Opt f(int cond)
{
    struct E* _Owner _Opt e = 0;
    try
    {
        e = make_a();
        if (e == 0) throw;

        struct T* _Opt pt = prev_token();
        if (pt == 0) throw;

        e->last_token = pt; /* remove this line and the warning disappears */

        e = tail(e);        /* moves the old pointee, e now points elsewhere */
        if (e == 0) throw;
    }
    catch
    {
        e_delete(e);
        e = 0;
    }
    return e;
}
