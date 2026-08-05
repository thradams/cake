#pragma safety enable

/*
   Regression test for a false positive reported directly:

     void f(int* a)
     {
         a++;
         *a = 1; // "possible null pointer dereference" -- false positive
     }

   Root cause: EXPR_POSTFIX_INCREMENT/DECREMENT and EXPR_UNARY_
   INCREMENT/DECREMENT computed the pointer's post-arithmetic
   alternative generically: any tracked value that wasn't a plain
   signed/unsigned integer (this includes every pointer alternative --
   whether a concrete non-null arena pointee or an _Opt null/non-null
   pair) fell into the same catch-all branch that degrades the value to
   a bare FLOW3_VALUE_KIND_SIGNED ANY. flow3_alternative_can_be_zero
   treats ANY as "could be zero", so an ordinary non-_Opt `int* a`
   parameter -- assumed non-null on entry, like every non-optional
   pointer parameter -- lost that guarantee the moment `a++;` ran, and
   the very next dereference incorrectly warned.

   Fixed by special-casing pointer-kind alternatives in the increment/
   decrement handling: keep the SAME alternative (same relation, same
   pointee/null identity) across the arithmetic, since advancing or
   retreating a valid pointer can never turn it into a null one (that
   can only happen through an explicit assignment). This also fixed the
   same false positive in a loop shape found in cake's own patterns
   (see safety-167.c: `while (*p) { p++; }`).
*/

void f(int* a)
{
    a++;
    *a = 1; /* ok: incrementing a non-optional pointer keeps it non-null */
}

void g(int* a)
{
    ++a;
    *a = 1; /* ok: prefix increment, same rule */
}

void h(int* a)
{
    a--;
    *a = 1; /* ok: decrement, same rule */
}

void loop(char* p)
{
    while (*p)
    {
        p++; /* ok: repeated increments still keep p non-null */
    }
}

/* Contrast: an _Opt pointer's null-or-non-null uncertainty must still
   survive pointer arithmetic -- incrementing it doesn't resolve
   whether it was null in the first place. */
void opt_still_warns(int* _Opt a)
{
    a++;
    *a = 1; //lint 33 possible null pointer dereference
}

/* Several increments in a row, straight-line (not a loop): each `p++`
   re-derives its alternative from the PREVIOUS one, so the "still
   non-null" fact must keep propagating across every step, not just
   survive a single increment. */
void many(int* p)
{
    p++;
    p++;
    p++;
    *p = 1; /* ok: still non-null after three increments */
}

void many_mixed(int* p)
{
    p++;
    p++;
    p--;
    *p = 1; /* ok: increments and decrements freely mixed */
}
