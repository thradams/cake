#pragma safety enable

/*
   Regression tests for flow3_visit_for_statement, checked with the same
   methodology used for while/do-while: dead code after an unconditional
   jump inside the body, zero-iteration exit, and nested for-loops'
   break/continue targeting the correct (innermost) loop.

   flow3_visit_for_statement shares the same two-pass warm-up/second-pass
   structure as while/do-while, so it needed (and got) the identical
   fixes: the current map is marked is_dead by flow3_visit_jump_statement
   on return/break/continue/goto/throw, merge_arms skips dead arms when
   building the post-loop state, and a body that diverges on every path
   gets a discarded diagnostics-only revisit so warning 68 "unreachable
   code" is not silently swallowed by the suppressed warm-up pass.
*/

void break_dead_code(int n)
{
    int a = 0;
    for (int i = 0; i < n; i++)
    {
        a = 1;
        break;
        a = 99; //lint 68 unreachable code
    }
    /* expected: a == 0 (zero iterations) or a == 1 (broke out); never 99 */
    // static_debug(a);
    compile_assert(a == 0 || a == 1);
}

void continue_dead_code(int n)
{
    int a = 0;
    for (int i = 0; i < n; i++)
    {
        a = 1;
        continue;
        a = 99; //lint 68 unreachable code
    }
    /* expected: a == 0 only -- continue always re-checks the condition
       before another iteration could set a == 1 and fall off the end */
    // static_debug(a);
    compile_assert(a == 0);
}

void return_dead_code(int n)
{
    int a = 0;
    for (int i = 0; i < n; i++)
    {
        a = 1;
        return;
        a = 99; //lint 68 unreachable code
    }
    /* expected: only reachable here with zero iterations, so a == 0 */
    // static_debug(a);
    compile_assert(a == 0);
}

void zero_iterations(int n)
{
    int a = 0;
    for (int i = 0; i < n; i++)
    {
        a = 1;
    }
    /* expected: a == 0 (loop never entered) or a == 1 (ran at least once) */
    // static_debug(a);
    compile_assert(a == 0 || a == 1);
}

void nested_break_targets_inner_loop(int n, int m)
{
    int a = 0;
    int b = 0;
    for (int i = 0; i < n; i++)
    {
        a = 1;
        for (int j = 0; j < m; j++)
        {
            b = 1;
            break;
        }
        /* expected: the inner break must not also break the outer loop --
           b == 0 (inner loop never entered) or b == 1 (broke out) */
        // static_debug(b);
        compile_assert(b == 0 || b == 1);
    }
    /* expected: the outer loop itself never broke, so a == 0 or a == 1
       exactly as in zero_iterations above */
    // static_debug(a);
    compile_assert(a == 0 || a == 1);
}
