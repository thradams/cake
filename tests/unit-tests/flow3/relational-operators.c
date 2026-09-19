#pragma safety enable

/* narrowing on >, >=, <, <=: the true branch records `var OP const`, the false branch the negation; every compile_assert here must be provable */

/* -------- assert records the exact half-line constraint -------- */

void assert_greater(int a)
{
    _Assert(a > 0);
    compile_assert(a > 0);
}

void assert_greater_equal(int a)
{
    _Assert(a >= 5);
    compile_assert(a >= 5);
}

void assert_less(int a)
{
    _Assert(a < 10);
    compile_assert(a < 10);
}

void assert_less_equal(int a)
{
    _Assert(a <= -1);
    compile_assert(a <= -1);
}

/* -------- constant on the left is normalized (c OP var == var OP' c) -------- */

void constant_on_left(int a)
{
    _Assert(0 < a);          /* equivalent to a > 0 */
    compile_assert(a > 0);
}

void constant_on_left_ge(int a)
{
    _Assert(10 >= a);        /* equivalent to a <= 10 */
    compile_assert(a <= 10);
}

/* -------- a stronger constraint implies weaker ones -------- */

void implication(int a)
{
    _Assert(a > 5);
    compile_assert(a > 0);   /* a > 5 implies a > 0  */
    compile_assert(a >= 1);  /* ...and a >= 1        */
    compile_assert(a >= 5);  /* ...and a >= 5        */
    compile_assert(a > 4);   /* ...and a > 4         */

    /* not provable: a > 6 (a could be 6), a > 10 */
}

/* -------- the false branch carries the negated constraint -------- */

void if_gt_false_branch(int a)
{
    if (a > 0)
    {
        compile_assert(a > 0);
    }
    else
    {
        compile_assert(a <= 0);  /* !(a > 0) is a <= 0 */
    }
}

void if_ge_false_branch(int a)
{
    if (a >= 3)
    {
        compile_assert(a >= 3);
    }
    else
    {
        compile_assert(a < 3);   /* !(a >= 3) is a < 3 */
    }
}

void if_lt_false_branch(int a)
{
   

   
}

/* -------- concrete constants still fold as before -------- */

void concrete_folds(void)
{
    int a = 7;
    compile_assert(a > 0);
    compile_assert(a >= 7);
    compile_assert(a < 8);
    compile_assert(a <= 7);
}

/* -------- soundness: an inclusive bound must not over-narrow -------- */

void boundary_inclusive(int a)
{
    _Assert(a >= 0);
    compile_assert(a >= 0);
    /* a > 0 is not provable here: a could be exactly 0 */
}

/* -------- assert with the value on both sides of zero -------- */

void negative_bound(int a)
{
    _Assert(a < 0);
    compile_assert(a < 0);
    compile_assert(a <= -1);   /* for ints, a < 0 implies a <= -1... */
    compile_assert(a < 1);     /* ...and certainly a < 1 */
}
