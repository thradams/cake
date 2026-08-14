#pragma safety enable

/*
   An `else if` must narrow from its OWN condition, whatever the preceding
   arm tested.

   When the first arm's condition is compound (`a > 0 && b > 0`), the else
   branch state was losing the refinement that the `else if` then
   establishes, so `b < 0` no longer proved `-b` non-zero and correct code
   reported "division by zero".

   Regression for object.c's signed_long_long_mul / signed_long_long_add,
   which are written exactly in this shape.
*/

/* Each ingredient on its own is fine. */

long long compound_only(long long a, long long b)
{
    if (a < 0 && b < 0)
    {
        return 100 / -b;
    }
    return 0;
}

long long else_if_only(long long a, long long b)
{
    if (a > 0)
    {
        return 1;
    }
    else if (b < 0)
    {
        return 100 / -b;
    }
    return 0;
}

long long compound_without_negation(long long a, long long b)
{
    if (a < 0 && b < 0)
    {
        return 100 / b;
    }
    return 0;
}

/* Compound first arm + else if + negated divisor. */
long long compound_then_else_if(long long a, long long b)
{
    if (a > 0 && b > 0)
    {
        return 1;
    }
    else if (b < 0)
    {
        return 100 / -b;
    }
    return 0;
}

/* The shape object.c actually uses: both arms compound. */
long long both_arms_compound(long long a, long long b)
{
    if (a > 0 && b > 0)
    {
        return 100 / b;
    }
    else if (a < 0 && b < 0)
    {
        return 100 / -b;
    }
    return 0;
}

/* A later `else` that rules the divisor out by an earlier return still
   narrows -- object.c's third reported line. */
long long else_after_early_return(long long a, long long b)
{
    if (a > 0 && b > 0)
    {
        return 1;
    }
    else if (a < 0 && b < 0)
    {
        return 2;
    }
    else
    {
        if (a == 0 || b == 0)
        {
            return 0;
        }
        if (b > 0)
        {
            return 100 / b;
        }
        return 100 / a;
    }
}
