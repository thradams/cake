#pragma safety enable

/*
   Regression test for a false positive reported directly:

     void f(int* a)
     {
         a += 1;
         *a = 1; // "possible null pointer dereference" -- false positive
     }

   Same class of bug already fixed for p++/p--/++p/--p (see
   pointer-arithmetic-preserves-nonnull.c), just reached through a
   different operator: EXPR_ASSIGNMENT_PLUS_ASSIGN/MINUS_ASSIGN/etc.
   only special-cased the "compute a new constant" path (both sides
   plain integers with known values); anything else -- including a
   pointer-kind destination -- fell into the generic "unknown" fallback
   that degraded the destination to a bare FLOW3_VALUE_KIND_SIGNED ANY
   value. flow3_alternative_can_be_zero treats ANY as "could be zero",
   so an ordinary non-optional `int* a` parameter lost its non-null
   guarantee right after `a += 1;`, even though pointer arithmetic can
   never turn a valid pointer into a null one.

   Fixed by keeping the SAME alternative for a pointer-kind destination
   across the compound assignment (same rationale as the increment/
   decrement fix): advancing or retreating a valid pointer doesn't
   change whether it's null.
*/

void f(int* a)
{
    a += 1;
    *a = 1; /* ok: += on a non-optional pointer keeps it non-null */
}

void g(int* a)
{
    a -= 1;
    *a = 1; /* ok: same rule for -= */
}

void h(int* a, int n)
{
    a += n;
    *a = 1; /* ok: same rule with a non-constant offset */
}

/* Contrast: an _Opt pointer's null-or-non-null uncertainty must still
   survive a compound assignment. */
void opt_still_warns(int* _Opt a)
{
    a += 1;
    *a = 1; //lint 33 possible null pointer dereference
}
