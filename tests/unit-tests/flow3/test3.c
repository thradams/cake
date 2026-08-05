#pragma flow enable

void f_rel_eq_then_gt_const(int x)
{
    if (x == 5)
    {
        compile_assert(x > 3);   /* 5 > 3 */
    }
}

void f_rel_eq_then_lt_const(int x)
{
    if (x == 5)
    {
        compile_assert(x < 10);  /* 5 < 10 */
    }
}

void f_rel_eq_then_ge_const(int x)
{
    if (x == 5)
    {
        compile_assert(x >= 5);  /* 5 >= 5 */
    }
}

void f_rel_eq_then_le_const(int x)
{
    if (x == 5)
    {
        compile_assert(x <= 5);  /* 5 <= 5 */
    }
}

void f_rel_const_lt_eq(int x)
{
    if (x == 5)
    {
        compile_assert(3 < x);   /* 3 < 5 */
    }
}

void f_rel_const_gt_eq(int x)
{
    if (x == 5)
    {
        compile_assert(10 > x);  /* 10 > 5 */
    }
}

void f_rel_const_le_eq(int x)
{
    if (x == 5)
    {
        compile_assert(5 <= x);  /* 5 <= 5 */
    }
}

void f_rel_const_ge_eq(int x)
{
    if (x == 5)
    {
        compile_assert(5 >= x);  /* 5 >= 5 */
    }
}

void f_rel_two_vars_lt(int x, int y)
{
    if (x == 3)
    {
        if (y == 7)
        {
            compile_assert(x < y);   /* 3 < 7 */
        }
    }
}

void f_rel_two_vars_le(int x, int y)
{
    if (x == 3)
    {
        if (y == 7)
        {
            compile_assert(x <= y);  /* 3 <= 7 */
        }
    }
}

void f_rel_two_vars_gt(int x, int y)
{
    if (x == 7)
    {
        if (y == 3)
        {
            compile_assert(x > y);   /* 7 > 3 */
        }
    }
}

void f_rel_two_vars_ge(int x, int y)
{
    if (x == 7)
    {
        if (y == 3)
        {
            compile_assert(x >= y);  /* 7 >= 3 */
        }
    }
}

void f_rel_two_vars_le_equal(int x, int y)
{
    if (x == 5)
    {
        if (y == 5)
        {
            compile_assert(x <= y);  /* 5 <= 5 */
        }
    }
}

void f_rel_two_vars_ge_equal(int x, int y)
{
    if (x == 5)
    {
        if (y == 5)
        {
            compile_assert(x >= y);  /* 5 >= 5 */
        }
    }
}

void f_rel_outer_ne_inner_eq_gt(int x)
{
    if (x != 0)
    {
        if (x == 5)
        {
            compile_assert(x > 0);   /* 5 > 0 */
        }
    }
}


void f_rel_outer_ne_inner_eq_lt(int x)
{
    if (x != 0)
    {
        if (x == 5)
        {
            compile_assert(x < 10);  /* 5 < 10 */
        }
    }
}

void f_rel_outer_ne_inner_eq_two_vars(int x, int y)
{
    if (x != 0)
    {
        if (x == 5)
        {
            if (y == 2)
            {
                compile_assert(x > y);   /* 5 > 2 */
            }
        }
    }
}


void f_rel_negative_lt_zero(int x)
{
    if (x == -3)
    {
        compile_assert(x < 0);   /* -3 < 0 */
    }
}

void f_rel_negative_two_vars(int x, int y)
{
    if (x == -3)
    {
        if (y == -1)
        {
            compile_assert(x < y);   /* -3 < -1 */
        }
    }
}

void f_rel_negative_gt(int x, int y)
{
    if (x == -1)
    {
        if (y == -3)
        {
            compile_assert(x > y);   /* -1 > -3 */
        }
    }
}

void f_rel_negative_const_left(int x)
{
    if (x == -5)
    {
        compile_assert(-10 < x);  /* -10 < -5 */
    }
}

void f_rel_zero_ge(int x)
{
    if (x == 0)
    {
        compile_assert(x >= 0);  /* 0 >= 0 */
    }
}

void f_rel_zero_le(int x)
{
    if (x == 0)
    {
        compile_assert(x <= 0);  /* 0 <= 0 */
    }
}

void f_rel_one_gt_zero(int x)
{
    if (x == 1)
    {
        compile_assert(x > 0);   /* 1 > 0 */
    }
}

void f_rel_neg_one_lt_zero(int x)
{
    if (x == -1)
    {
        compile_assert(x < 0);   /* -1 < 0 */
    }
}

void f_rel_three_vars_skip(int x, int y, int z)
{
    if (x == 1)
    {
        if (y == 2)
        {
            if (z == 3)
            {
                compile_assert(x < z);   /* 1 < 3  (y not used) */
            }
        }
    }
}

void f_rel_three_vars_middle(int x, int y, int z)
{
    if (x == 10)
    {
        if (y == 20)
        {
            if (z == 30)
            {
                compile_assert(y < z);   /* 20 < 30 */
            }
        }
    }
}
