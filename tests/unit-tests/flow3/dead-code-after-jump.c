#pragma safety enable

/*
   Regression test: code that syntactically follows an unconditional
   jump (return/break/continue/goto/throw) in the SAME block is dead --
   it can never execute -- so its effects must not leak into whatever
   merge later combines that arm with the others.

   Previously: flow3_visit_if_statement (and try/while/do/for) decided
   whether a branch "reaches the end" using secondary_block_ends_with_jump,
   a purely syntactic check of the LAST statement in the block. It never
   noticed an EARLIER unconditional jump followed by more (dead) code, so
   the branch was treated as if it fell through normally, and the dead
   code's writes were merged into the post-branch state.

   Fixed: flow3_visit_jump_statement now marks the current map is_dead
   right when handling return/break/continue/goto/throw (mirroring the
   existing is_dead mechanism already used for constant-folded dead
   branches). merge_arms already skips is_dead arms unconditionally, so
   dead code after a jump can no longer leak into a merge, regardless of
   what the syntax-based check believed.

   Since this now lets flow3 tell dead code apart from live code,
   flow3_visit_block_item_list also reports it: warning 68
   "unreachable code" fires once on the first statement of each dead
   run (a later label that is a real goto target resets detection, so a
   further jump can still start a new dead-code warning after it).

   For while/do/for, a body that diverges on every path only gets a
   diagnostics-suppressed "warm up" pass under the normal two-pass
   scheme (the real second pass is skipped, since there is no sound
   "after one iteration" state to build on top of a dead map) -- so
   each loop below re-visits such a body once more, fresh and with
   diagnostics on, purely so this warning is not silently swallowed;
   the revisit's resulting state is discarded either way.

   Below, `a = 99;` after `return;` is unreachable. Only a == 1 should
   ever be observed after the call to f.
*/

void f(int x)
{
    int a = 1;
    if (x)
    {
        return;
        a = 99; //lint 68 unreachable code
    }
    /* expected: a == 1 only -- the dead "a = 99" must not appear */
    // static_debug(a);
    compile_assert(a == 1);
}

void g(int x)
{
    int a = 0;
    while (x)
    {
        a = 1;
        break;
        a = 99; //lint 68 unreachable code
    }
    /* expected: a == 0 (never entered) or a == 1 (broke out); never 99 */
    // static_debug(a);
    compile_assert(a == 0 || a == 1);
}

void h(int x)
{
    int a = 0;
    while (x)
    {
        a = 1;
        continue;
        a = 99; //lint 68 unreachable code
    }
    /* expected: a == 0 only -- continue re-checks the condition, and the
       first pass's dead tail must not pollute the second pass either */
    // static_debug(a);
    compile_assert(a == 0);
}

void k(int x)
{
    int a = 0;
    if (x)
    {
        a = 1;
        goto L;
        a = 99; //lint 68 unreachable code
    }
L:
    /* expected: a == 0 (fall-through) or a == 1 (via goto); never 99 */
    // static_debug(a);
    compile_assert(a == 0 || a == 1);
}

void m(int x)
{
    int a = 0;
    try
    {
        a = 1;
        throw;
        a = 99; //lint 68 unreachable code
    }
    catch
    {
        /* expected: a == 1 only -- the dead "a = 99" must not appear */
        // static_debug(a);
        compile_assert(a == 1);
    }
}
