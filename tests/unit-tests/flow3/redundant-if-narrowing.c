#pragma safety enable

/*
   Redundant nested guard: `if (p) { if (p) { ... } }`. The state of p
   INSIDE the second (redundant) if must stay non-null -- the inner if
   re-tests a fact the outer if already established, and narrowing must
   still see that fact.
*/

void inner_if_sees_nonnull(int* _Opt p)
{
    if (p)
    {
        if (p)
        {
            *p = 1; /* ok: p is non-null here, proven twice over */
        }
    }
}

/*
   KNOWN GAP (not fixed here -- see below): code AFTER a redundant,
   empty/no-else inner `if (p) {}` loses p's non-null narrowing, even
   though nothing in the inner if could possibly have invalidated p.

     if (p)
     {
         if (p) {}
         *p = 1; // currently warns "possible null pointer dereference" -- false positive
     }

   Root cause: flow3_narrow_map_into's FALSE-arm handling for an
   already-known-nonzero value ("value != 0", `flow3_value_is_true(alt) &&
   !true_branch`, and the equivalent NOT_EQUAL-relation case just above it)
   deliberately synthesizes a contradictory "== 0" fact for that arm instead
   of proving it unreachable, because in a DIFFERENT and legitimate shape --
   `while (it) { ...; it = next; }` -- the loop body reassigns the value each
   iteration, so by loop exit "== 0" is genuinely possible, not a
   contradiction. flow3_narrow_map_into has no way, from the alternative
   alone, to tell "this exact fact was just reasserted, redundantly" (this
   sample) apart from "this value was reassigned since it was last true"
   (the while-loop case) -- the same code path serves both, and only one of
   the two shapes is actually reachable.

   A fix attempt marking the contradictory arm's map `is_unreachable = true`
   (so flow3_map_merge_arms's existing dead-arm skip would exclude it) was
   tried and reverted: it broke the while-loop-exit case and produced false
   "unreachable code" diagnostics in unrelated samples (correlation-checkpoint.c,
   owner-if-else-050.c, owner-resource-105.c), because that same code path is legitimately
   reached from reassignment-driven narrowing, not just from redundant
   re-testing. Distinguishing the two needs the narrowing call site to say
   whether the value could have changed since the fact was established
   (similar in spirit to flow3_predicate_shared_id's same-predicate
   correlation for `if`/`if` pairs, extended to cover `if`/`while` and
   assignment-invalidation) -- left for a future, more careful fix.
*/
void after_redundant_if_loses_nonnull(int* _Opt p)
{
    if (p)
    {
        if (p)
        {
        }
        *p = 1; //lint 33 known false positive: possible null pointer dereference
    }
}
