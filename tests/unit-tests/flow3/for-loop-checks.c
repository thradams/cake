#pragma safety enable

/* for loops: dead code after a jump in the body, zero-iteration exit, nested break/continue target the innermost loop */

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
    /* expected a == 0 only: continue re-checks the condition before a == 1 could fall off the end */
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
        /* expected b == 0 (inner loop never entered) or b == 1 (broke out), the inner break must not break the outer loop */
        // static_debug(b);
        compile_assert(b == 0 || b == 1);
    }
    /* expected a == 0 or a == 1: the outer loop never broke */
    // static_debug(a);
    compile_assert(a == 0 || a == 1);
}
