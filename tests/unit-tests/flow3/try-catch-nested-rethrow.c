#pragma safety enable

/* a throw inside a catch propagates to the enclosing try, not into the same try's join: the outer catch sees a == 3 */

void f(int x)
{
    int a = 0;
    try
    {
        try
        {
            a = 1;
            if (x)
                throw;
            a = 2;
        }
        catch
        {
            a = 3;
            throw;
        }
    }
    catch
    {
        /* expected: a == 3 (the inner catch's value, via re-throw) */
        compile_assert(a == 3);
    }
}
