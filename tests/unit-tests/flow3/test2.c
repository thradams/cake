#pragma safety enable


void f_eq_nonzero_true_branch(int x)
{
    if (x == 3)
    {
        compile_assert(x == 3);
    }
}

void f_eq_nonzero_false_branch(int x)
{
    if (x == 3)
    {
        /* nothing */
    }
    else
    {
        compile_assert(x != 3);
    }
}

void f_eq_one_true_branch(int x)
{
    if (x == 1)
    {
        compile_assert(x == 1);
    }
}

void f_eq_neg_true_branch(int x)
{
    if (x == -1)
    {
        compile_assert(x == -1);
    }
}


void f_neq_nonzero_true_branch(int x)
{
    if (x != 2)
    {
        compile_assert(x != 2);          /* was failing after first fix */
    }
}

void f_neq_nonzero_false_branch(int x)
{
    if (x != 2)
    {
        /* nothing */
    }
    else
    {
        compile_assert(x == 2);          /* false branch of !=: x must be 2 */
    }
}

void f_neq_one_true_branch(int x)
{
    if (x != 1)
    {
        compile_assert(x != 1);
    }
}

void f_compile_assert_same_cond_eq(int x)
{
    if (x == 5)
    {
        compile_assert(x == 5);
        compile_assert(x != 4);          /* 5 != 4 */
        compile_assert(x != 0);          /* 5 != 0 */
    }
}

void f_compile_assert_same_cond_neq(int x)
{
    if (x != 7)
    {
        compile_assert(x != 7);
    }
}

void f_two_alternatives(int c)
{
    int a;
    if (c)
        a = 1;
    else
        a = 2;

    compile_assert(a == 1 || a == 2);
    compile_assert(a != 3);
    compile_assert(a != 0);
}

void f_three_alternatives(int c)
{
    int a;
    if (c == 1)
        a = 10;
    else if (c == 2)
        a = 20;
    else
        a = 30;

    compile_assert(a == 10 || a == 20 || a == 30);
    compile_assert(a != 0);
}

void f_same_value_both_branches(int c)
{
    int a;
    if (c)
        a = 5;
    else
        a = 5;

    compile_assert(a == 5);
}

void f_or_two_alternatives(int c)
{
    int a;
    if (c)
        a = 1;
    else
        a = 2;

    compile_assert(a == 1 || a == 2);
}

void f_or_with_known_true_left(int x)
{
    if (x == 4)
    {
        /* left is true, OR is true */
        compile_assert(x == 4 || x == 9);   
    }
}

void f_or_with_known_true_right(int x)
{
    if (x == 9)
    {
        /* right is true, OR is true */
        compile_assert(x == 4 || x == 9);   
    }
}


void f_and_both_known(int x)
{
    if (x == 6)
    {
        /* 6==6 true, 6!=7 true */
        compile_assert(x == 6 && x != 7);   
    }
}

void f_and_short_circuit(int x)
{
    if (x == 0)
    {
        compile_assert(x == 0 && x != 1);
    }
}


void f_nested_eq(int x)
{
    if (x == 3)
    {
        if (x == 3)
        {
            /* always true inside outer true branch */
            compile_assert(x == 3);
        }
    }
}

void f_inner_refine(int x)
{
    /* outer narrows to != 0; inner narrows further to == 5 */
    if (x != 0)
    {
        if (x == 5)
        {
            compile_assert(x == 5);
            compile_assert(x != 0);
        }
    }
}


void f_commuted_eq(int x)
{
    if (3 == x)
    {
        compile_assert(x == 3);
        compile_assert(3 == x);
    }
}

void f_commuted_neq(int x)
{
    if (2 != x)
    {
        compile_assert(x != 2);
    }
}

void f_eq_zero_true_branch(int x)
{
    if (x == 0)
    {
        compile_assert(x == 0);
    }
}

void f_eq_zero_false_branch(int x)
{
    if (x == 0)
    {
        /* nothing */
    }
    else
    {
        compile_assert(x != 0);
    }
}

void f_neq_zero_true_branch(int x)
{
    if (x != 0)
    {
        compile_assert(x != 0);
    }
}

void f_neq_zero_false_branch(int x)
{
    if (x != 0)
    {
        /* nothing */
    }
    else
    {
        compile_assert(x == 0);
    }
}

void f_sequential_eq(int x, int y)
{
    if (x == 3)
    {
        if (y == 7)
        {
            compile_assert(x == 3);
            compile_assert(y == 7);
            compile_assert(x != y);      /* 3 != 7 */
        }
    }
}

void f_sequential_neq(int x, int y)
{
    if (x != 3)
    {
        if (y != 3)
        {
            compile_assert(x != 3);
            compile_assert(y != 3);
        }
    }
}

void f_neq_different_constants(int x)
{
    if (x != 5)
    {
        /* x != 5, but x could be 3 or not — compile_assert(x != 3) must NOT pass */
        (void)x;
    }
}

void f_neq_same_constant_twice(int x)
{
    if (x != 4)
    {
        compile_assert(x != 4);         /* same constant: must fold to true */
    }
}

void f_tautology_from_alternatives(int c)
{
    int a;
    if (c)
        a = 0;
    else
        a = 1;

    /* Every alternative (0 and 1) satisfies a == 0 || a == 1 */
    compile_assert(a == 0 || a == 1);

    /* Every alternative satisfies a != 2 */
    compile_assert(a != 2);
}

void f_eq_after_known_assign(int x)
{
    int a = 42;
    compile_assert(a == 42);
    compile_assert(a != 0);
    compile_assert(a != 41);
    compile_assert(a != 43);
    (void)x;
}
