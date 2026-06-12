/*
 * flow3 loop exit-state tests
 * Each function documents the expected value of variables after the loop.
 * Use static_debug(var) to inspect; the comment states the expected result.
 */

#pragma safety enable

 /* -----------------------------------------------------------------------
  * while
  * ----------------------------------------------------------------------- */

  /* Basic countdown: after loop a must be 0 */
void test_while_basic(int a)
{
    while (a)
    {
        a--;
    }
    static_debug(a); // expected: a == 0
}

/* Post-decrement condition: while (a--) decrements before test;
 * when loop exits the pre-decrement value was 0, so a == -1 after.
 * The analyser tracks the object 'a' used as the condition,
 * which after the last decrement is 0 (the value that made the
 * condition false). */
void test_while_post_decrement(int a)
{
    while (a--)
    {
    }
    static_debug(a); // expected: a == 0  (value at point condition was false)
}

/* Loop that never executes when a == 0 on entry.
 * Exit via false branch: a == 0. */
void test_while_may_not_execute(int a)
{
    while (a)
    {
        a--;
    }
    static_debug(a); // expected: a == 0
}

/* break inside loop — break-join carries the state at the break point.
 * After loop: a can be != 0 (broke out) or 0 (condition became false). */
void test_while_break(int a)
{
    while (a)
    {
        if (a == 5)
            break;
        a--;
    }
    static_debug(a); // expected: a == 0 or a == 5
}

/* Loop with no modification to condition variable —
 * if a != 0 on entry the loop is infinite; analyser should not
 * narrow a to 0 on the false branch (it is unreachable).
 * Only reachable exit is break. */
void test_while_infinite_with_break(int a)
{
    while (a)
    {
        break;
    }
    static_debug(a); // expected: a is any (not narrowed, break carries original value)
}

/* -----------------------------------------------------------------------
 * do-while
 * ----------------------------------------------------------------------- */

 /* Basic countdown: body runs at least once, exits when a == 0 */
void test_do_while_basic(int a)
{
    do
    {
        a--;
    } while (a);
    static_debug(a); // expected: a == 0
}

/* Condition checked after body: even if a == 0 on entry,
 * body runs once (a becomes -1), then condition is false. */
void test_do_while_zero_entry(void)
{
    int a = 0;
    do
    {
        a--;
    } while (a);
    static_debug(a); // expected: a == -1  (ran once from 0)
}

/* break inside do-while */
void test_do_while_break(int a)
{
    do
    {
        if (a == 1)
            break;
        a--;
    } while (a);
    static_debug(a); // expected: a == 0 or a == 1
}

/* -----------------------------------------------------------------------
 * for
 * ----------------------------------------------------------------------- */

 /* Classic counted loop: after loop i == n */
void test_for_basic(int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
    }
    static_debug(i); // expected: i == n  (condition i < n was false)
}

/* Countdown for loop */
void test_for_countdown(int a)
{
    for (; a > 0; a--)
    {
    }
    static_debug(a); // expected: a == 0
}

/* for loop with break */
void test_for_break(int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        if (i == 5)
            break;
    }
    static_debug(i); // expected: i == n or i == 5
}

/* for loop with no condition (infinite) and break */
void test_for_no_condition(int a)
{
    for (;;)
    {
        if (a == 0)
            break;
        a--;
    }
    static_debug(a); // expected: a == 0
}