#pragma safety enable

/*
   False "unreachable code" (warning 68) on the else-branch assignment when
   the condition is written as `p != NULL` (or `p != 0`) inside a loop, for
   a nullable pointer that starts NULL and gets assigned on some iteration.
   Rewriting the exact same condition as plain pointer truthiness (`if (p)`)
   makes the false positive disappear.

   The `!=` narrowing is NOT the cause, despite that clue. This is the same
   defect as loop-second-pass-narrows-first-iteration.c: the second
   (diagnostic) pass starts from the first pass's exit state, where p has
   already been assigned, so `p != NULL` folds to true and the else-branch
   looks unreachable. The first iteration reaches it.

   FIXED. flow_visit_while_statement now unions the pre-loop value back into
   the state the diagnostic pass starts from -- but per key, and only for keys
   carrying no ownership or lifetime facts (flow_join_first_iteration_values).
   The blanket "zero iterations" arm the `for` case uses could not be copied
   here: flow_map_merge_arms inherits the parent's value for every key an arm
   did not write, which re-introduces pre-loop owner state and breaks the
   free-and-advance idiom. See that function's comment for the measurement.

   Reduced from src/expressions.c's generic-selection parsing loop
   (p_default_generic_association_first_token), around line 548, where
   changing `if (p_default_generic_association_first_token != NULL)` to
   `if (p_default_generic_association_first_token)` made the warning go
   away. User-reported.
*/

#define NULL ((void*)0)

void f(int cond)
{
    int* _Opt p = 0;

    while (cond)
    {
        if (p != NULL)
        {
        }
        else
        {
            p = &cond; /* reachable: the first iteration has p == 0 */
        }
    }
}
