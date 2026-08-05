    #pragma flow enable

void f_logical_and_two_eq(int x, int y)
{
    if (x == 3 && y == 7)
    {
        compile_assert(x == 3);   /* left narrowing active */
        compile_assert(y == 7);   /* right narrowing active */
        compile_assert(x != y);   /* 3 != 7 — two-var equality */
        compile_assert(x < y);    /* 3 < 7  — two-var relational */
    }
}

void f_logical_and_same_value(int x, int y)
{
    if (x == 5 && y == 5)
    {
        compile_assert(x == y);   /* 5 == 5 */
        compile_assert(x <= y);   /* 5 <= 5 */
        compile_assert(x >= y);   /* 5 >= 5 */
    }
}

void f_logical_and_three(int x, int y, int z)
{
    if (x == 1 && y == 2 && z == 3)
    {
        compile_assert(x < y);    /* 1 < 2 */
        compile_assert(y < z);    /* 2 < 3 */
        compile_assert(x < z);    /* 1 < 3 */
    }
}

void f_logical_and_rel_then_eq(int x)
{
    if (x > 0 && x == 5)
    {
        compile_assert(x == 5);   /* right side narrows to exactly 5 */
        compile_assert(x > 0);    /* 5 > 0 */
        compile_assert(x > 3);    /* 5 > 3 */
    }
}

void f_logical_and_ne_then_eq(int x)
{
    if (x != 0 && x == 7)
    {
        compile_assert(x == 7);   /* right fully narrows */
        compile_assert(x > 0);    /* 7 > 0 */
    }
}

void f_logical_and_result_true(int x, int y)
{
    if (x == 3)
    {
        if (y == 7)
        {
            compile_assert(x == 3 && y == 7);   /* both known true */
        }
    }
}

void f_logical_and_result_single_var(int x)
{
    if (x == 3)
    {
        compile_assert(x == 3 && x > 0);   /* 3==3 && 3>0 */
    }
}

void f_logical_or_false_branch(int x, int y)
{
    if (x == 3 || y == 7)
    {
        /* true branch: x==3 OR y==7 — too broad to assert specific values */
    }
    else
    {
        /* false branch: neither condition held */
        compile_assert(x != 3);   /* x was not 3 */
        compile_assert(y != 7);   /* y was not 7 */
    }
}

void f_logical_or_false_both_nonzero(int x, int y)
{
    if (x == 0 || y == 0)
    {
    }
    else
    {
        compile_assert(x != 0);
        compile_assert(y != 0);
    }
}

void f_logical_or_result_left_true(int x)
{
    if (x == 5)
    {
        compile_assert(x == 5 || x == 99);   /* left is true -> whole expr true */
    }
}

void f_logical_or_result_known_true(int x, int y)
{
    if (x == 3)
    {
        if (y == 7)
        {
            compile_assert(x == 3 || y == 99);  /* left true, right irrelevant */
        }
    }
}

void f_logical_not_eq(int x)
{
    if (!(x == 0))
    {
        compile_assert(x != 0);
    }
}

void f_logical_and_inside_outer_ne(int x, int y)
{
    if (x != 0)
    {
        if (x == 4 && y == 9)
        {
            compile_assert(x == 4);
            compile_assert(y == 9);
            compile_assert(x < y);    /* 4 < 9 */
            compile_assert(x != y);   /* 4 != 9 */
        }
    }
}

void f_logical_and_three_vars_nested(int x, int y, int z)
{
    if (x == 10)
    {
        if (y == 20 && z == 5)
        {
            compile_assert(z < x);    /* 5 < 10 */
            compile_assert(z < y);    /* 5 < 20 */
            compile_assert(x < y);    /* 10 < 20 */
        }
    }
}
