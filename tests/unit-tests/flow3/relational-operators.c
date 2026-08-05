#pragma safety enable

/*
   Tests for flow3 narrowing of the relational operators >, >=, <, <=.

   When a scalar variable is compared against a constant and the result is
   not constant-foldable, flow3 records a half-line constraint on the
   variable: the true branch gets `var OP const`, the false branch gets the
   negated relation. runtime_assert(expr) keeps only the true branch, so after
   `runtime_assert(a > 0)` the fact `a > 0` holds.

   Every compile_assert below is a POSITIVE check: it only passes if flow3
   can prove the condition from the recorded constraints, so this file must
   compile with 0 errors and 0 warnings. Facts flow3 cannot (soundly) prove
   are shown commented-out with the reason, to document the boundaries.
*/

/* -------- assert records the exact half-line constraint -------- */

void assert_greater(int a)
{
    runtime_assert(a > 0);
    compile_assert(a > 0);
}

void assert_greater_equal(int a)
{
    runtime_assert(a >= 5);
    compile_assert(a >= 5);
}

void assert_less(int a)
{
    runtime_assert(a < 10);
    compile_assert(a < 10);
}

void assert_less_equal(int a)
{
    runtime_assert(a <= -1);
    compile_assert(a <= -1);
}

/* -------- constant on the left is normalized (c OP var == var OP' c) -------- */

void constant_on_left(int a)
{
    runtime_assert(0 < a);          /* equivalent to a > 0 */
    compile_assert(a > 0);
}

void constant_on_left_ge(int a)
{
    runtime_assert(10 >= a);        /* equivalent to a <= 10 */
    compile_assert(a <= 10);
}

/* -------- a stronger constraint implies weaker ones -------- */

void implication(int a)
{
    runtime_assert(a > 5);
    compile_assert(a > 0);   /* a > 5 implies a > 0  */
    compile_assert(a >= 1);  /* ...and a >= 1        */
    compile_assert(a >= 5);  /* ...and a >= 5        */
    compile_assert(a > 4);   /* ...and a > 4         */

    /* Not provable -- flow3 correctly leaves these unknown:
         compile_assert(a > 6);    // a could be exactly 6
         compile_assert(a > 10);   // a could be 6
    */
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
    runtime_assert(a >= 0);
    compile_assert(a >= 0);
    /* a > 0 is deliberately NOT provable here: a could be exactly 0.
         compile_assert(a > 0);   // correctly rejected
    */
}

/* -------- assert with the value on both sides of zero -------- */

void negative_bound(int a)
{
    runtime_assert(a < 0);
    compile_assert(a < 0);
    compile_assert(a <= -1);   /* for ints, a < 0 implies a <= -1... */
    compile_assert(a < 1);     /* ...and certainly a < 1 */
}
