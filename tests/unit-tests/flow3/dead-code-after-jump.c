#pragma safety enable

/* code after an unconditional jump in the same block is dead: its writes never merge and warning 68 fires once per dead run */

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
    /* expected a == 0 only: the first pass's dead tail must not pollute the second pass */
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
