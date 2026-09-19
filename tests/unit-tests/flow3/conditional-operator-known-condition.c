#pragma safety enable

void takes_non_null(const char* s);

void known_true_comparison(void)
{
    int c = 1;
    int x = (c == 1) ? 10 : 20;
    compile_assert(x == 10); /* only the true arm */
}

void known_false_comparison(void)
{
    int c = 1;
    int x = (c == 2) ? 10 : 20;
    compile_assert(x == 20); /* only the false arm */
}

void narrowed_by_enclosing_if(int a)
{
    if (a == 0)
    {
        int x = (a == 0) ? 5 : 6;
        int y = (a != 0) ? 5 : 6;
        compile_assert(x == 5);
        compile_assert(y == 6);
    }
    else
    {
        int x = (a == 0) ? 5 : 6;
        compile_assert(x == 6);
    }
}

void unknown_condition_keeps_both(int c)
{
    int x = (c == 1) ? 10 : 20;
    compile_assert(x == 10 || x == 20);
}

void string_arms_are_not_null(int c)
{
    takes_non_null(c ? "a" : "b"); /* no null warning */

    const char* s = c ? "{}" : "[]";
    takes_non_null(s); /* no null warning */
}
