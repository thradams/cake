#pragma safety enable

/*
   Unary minus must carry the operand's RELATION through, mirrored:

       x >  v   =>   -x <  -v
       x >= v   =>   -x <= -v
       x <  v   =>   -x >  -v
       x <= v   =>   -x >= -v
       x != v   =>   -x != -v

   flow3 seeded the result of a non-constant `-x` as a plain ANY, throwing away
   everything known about x. So inside `if (b < 0)`, the divisor `-b` -- which
   is strictly positive and therefore never zero -- read as "could be anything,
   including 0" and produced a false "division by zero".

   (Reproduced from object.c signed_long_long_mul: `if (-a > LLONG_MAX / -b)`.)

   EQUAL is deliberately not carried through: a genuine constant is already
   folded earlier, and an EQUAL coming from flow state may be a stale seed.
*/

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

/*
   Documented (still warns, correctly): nothing is known about b, so -b may
   be zero.

       long long unguarded(long long b) { return LLONG_MAX / -b; }
*/
