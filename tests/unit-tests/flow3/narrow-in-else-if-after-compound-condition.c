#pragma safety enable

/* an `else if` after a compound condition narrows from its own condition, so `b < 0` proves -b non-zero (object.c signed_long_long_mul) */

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

/* a later else after an early return still narrows the divisor */
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
