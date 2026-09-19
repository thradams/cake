#pragma safety enable

/* `-x` mirrors x's relation (x > v gives -x < -v, etc.), so inside `if (b < 0)` the divisor -b is not zero (object.c signed_long_long_mul); EQUAL is not carried */

#define LLONG_MAX 9223372036854775807LL

/* b < 0  =>  -b > 0  =>  safe divisor. */
long long div_by_negated(long long b)
{
    if (b < 0)
        return LLONG_MAX / -b;   /* must NOT warn "division by zero" */
    return 0;
}

/* Contrast: the un-negated form already worked. */
long long div_by_positive(long long b)
{
    if (b > 0)
        return LLONG_MAX / b;    /* ok */
    return 0;
}

/* documented: with nothing known about b, `LLONG_MAX / -b` still warns */
