#pragma safety enable

/*
   Regression test for a false positive found in cake's own source
   (a Windows console helper shaped like this):

     bool enable_vt_mode(void)
     {
         ...
         if (h_out != INVALID_HANDLE_VALUE &&
             GetConsoleMode(h_out, &mode) != 0 &&
             SetConsoleMode(h_out, mode) != 0 &&
             SetConsoleOutputCP(CP_UTF8) != 0)
         {
             return true;
         }
         return false; // reported as unreachable code -- false positive
     }

   Minimal repro: `if (f1() && f1()) { return 1; } return 0;` warned
   "unreachable code" on the final return, even though the if's
   condition is not a compile-time constant and the code obviously runs
   whenever the condition is false.

   Root cause: for a non-narrowing operand like a plain function call,
   flow3_visit_expression returns the "identity pair" (p_true == p_false
   == the live current map, passed straight through unchanged -- see
   flow3_ensure_branch_pair's comment). EXPR_LOGICAL_AND's own result
   for `L && R` reuses R's true-branch pair as-is for its own p_true:
   when R is a plain call, that is genuinely still the SAME live map
   object the whole expression started with (call it M), not a fresh
   child scoped to "L and R were both true". flow3_ensure_branch_pair
   only forced fresh child maps when p_true and p_false were literally
   the same pointer; here p_false was already a distinct, freshly
   merged map (from the `&&`'s own false-arm merge), so the aliasing of
   p_true with the ORIGINAL pre-if map M slipped through unnoticed.

   The if-statement then visits its true-branch body directly on that
   aliased map M. The body's `return 1;` marks "the current map" dead --
   which is actually M, the pre-if ancestor map itself, not a scoped
   child. Since M is also an ancestor of the if's false-branch map and
   of whatever comes after the whole if, that stray is_dead=true bled
   out and made the following, perfectly reachable `return 0;` look
   like it was in a dead-code run.

   Fixed by having flow3_ensure_branch_pair also detect (and wrap in a
   fresh child map) the case where just ONE side of an already-distinct
   pair aliases the incoming pre-branch map directly, not only the
   p_true==p_false identity case.
*/

int f1(void);

int use(void)
{
    if (f1() && f1())
    {
        return 1;
    }
    return 0; /* ok: reachable, must NOT warn "unreachable code" */
}

int use3(void)
{
    if (f1() && f1() && f1())
    {
        return 1;
    }
    return 0; /* ok: same false positive, one more && link */
}

/* Contrast: a genuine unconditional jump followed by real dead code is
   still correctly flagged. */
void real_unreachable(void)
{
    return;
    int x = 1; //lint 68 unreachable code
    (void)x;
}
