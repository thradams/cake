#pragma safety enable

/*
   Regression test for a false positive reported directly against real
   code:

     unsigned char* psz = (unsigned char*)(current->lexeme + 0);

   Same class of bug already fixed for p++/p--/p+=n/p-=n (see
   pointer-arithmetic-preserves-nonnull.c and
   pointer-compound-assignment-preserves-nonnull.c), reached through the
   plain binary `+`/`-` operator this time:
   flow3_evaluate_binary_arithmetic only computed an exact result when
   BOTH operands were plain signed/unsigned integers; a pointer operand
   (on either side, for `+`) fell straight through to the generic
   fallback, which always builds a bare FLOW3_VALUE_KIND_SIGNED ANY
   value regardless of operand kind. flow3_alternative_can_be_zero
   treats ANY as "could be zero", so an ordinary non-optional pointer
   lost its non-null guarantee the moment it went through `+`/`-` with
   an integer offset, even though offsetting a valid pointer can never
   turn it into a null one.

   Fixed by keeping the pointer operand's own alternative unchanged for
   `ptr + int`, `int + ptr`, and `ptr - int` (not `ptr - ptr`, which is a
   genuinely different operation producing an integer, not a pointer).
*/

void f1(char* p)
{
    unsigned char* psz = (unsigned char*)(p + 0);
    *psz = 1; /* ok: p + 0 is still non-null */
}

void f2(char* p)
{
    unsigned char* psz = (unsigned char*)(p + 5);
    *psz = 1; /* ok: same rule with a nonzero offset */
}

void f3(char* p)
{
    unsigned char* psz = (unsigned char*)(p - 1);
    *psz = 1; /* ok: same rule for subtraction */
}

void f4(char* p)
{
    unsigned char* psz = (unsigned char*)(3 + p);
    *psz = 1; /* ok: same rule, integer on the left */
}

/* Contrast: an _Opt pointer's null-or-non-null uncertainty must still
   survive pointer + integer arithmetic. */
void opt_still_warns(char* _Opt p)
{
    unsigned char* psz = (unsigned char*)(p + 0); //lint 35 passing a possible null pointer '(unsignedchar*)(p+0)' to non-nullable pointer parameter
    *psz = 1; //lint 33 possible null pointer dereference
}
