#pragma safety enable

/*
   False "unreachable code" (warning 68) on the FIRST statement of a `while`
   body, when the only way back to the top of the loop is a `continue` taken
   from a branch that narrowed the value the first statement tests.

   Same root cause as loop-second-pass-narrows-first-iteration.c and
   unreachable-false-positive-ne-null.c -- the second (diagnostic) pass starts
   from a single predecessor state instead of the union over iterations -- but
   reached through a new edge. Here nothing is reassigned at all: the state
   that poisons the second pass is the NARROWING carried by the continue edge.

   Walking it through: the loop can only repeat via the `continue` in the
   `t == 2` branch, and on that edge flow3 knows `t != 1` (the first `if` was
   false) and `t == 2`. It then re-enters the body with only that state, so
   `t == 1` folds to false and the `return 1;` looks unreachable. The union
   with the loop's ENTRY state -- where t is unconstrained -- is what makes it
   reachable, and it is exactly that union the second pass drops. The first
   iteration reaches it for any t == 1.

   Removing the `continue` (letting control fall to the bottom of the body)
   makes the false positive disappear, which is the clue that it is the
   continue EDGE and not the narrowing itself.

   Note the difference from the other files in this folder: those need a
   variable that changes across iterations, so widening the changing value was
   at least a plausible (if unsound) fix. Here there is no such variable, so
   any fix has to join the continue edge with the loop-entry state rather than
   widen anything -- more evidence that the real fix is the "union over
   predecessors" one the whole family is waiting for.

   Reduced from src/expressions.c's expression_named_declarator, which walked
   a parenthesis/cast chain with `while` + `continue` and got its first `if`
   reported as unreachable. Rewritten there as straight-line recursion to
   dodge this, so the shape no longer occurs in the codebase -- kept here
   because the defect did.

   FIXED. Every path through this body diverges (`return`/`continue`/`break`),
   so flow_visit_while_statement takes its "re-visit the body once more purely
   so diagnostics can fire" path -- and that re-visit used to evaluate the loop
   condition from whatever map the first pass had left in
   ctx->p_current_flow_map, which is the body's own state carrying the
   continue edge's `t == 2`. It now restarts from p_before, the pre-loop
   state, so the first `if` is reachable again. The union-over-predecessors
   fix the header describes is still what the REST of this family needs; this
   case only ever needed the re-visit to start from the right map.
*/

struct e
{
    int t;
    struct e* _Opt next;
};

int first_statement_wrongly_unreachable(const struct e* _Opt p)
{
    while (p != 0)
    {
        if (p->t == 1) /* reachable: the first iteration constrains nothing */
        {
            return 1;
        }

        if (p->t == 2)
        {
            continue;
        }

        break;
    }

    return 0;
}
