#pragma safety enable

/*
   `bool e = (a || b);` -- the result of a logical operator is ALWAYS 0 or 1,
   so a variable initialized from it is initialized. FIXED; it used to be
   reported as possibly uninitialized.

   Found in cake's own tokenizer.c:4830:

       const bool results_in_empty_substituition =
           (r4.head == NULL || r4.head->type == TK_PLACEMARKER);
       ...
       return results_in_empty_substituition;   // "possibly uninitialized"

   Cause: EXPR_LOGICAL_OR seeds a precise per-path boolean only when BOTH sides
   are clean per-path booleans. Otherwise -- here the right operand dereferences
   a pointer whose nullability differs per path -- the result is left UNSEEDED.
   An object with no flow state reads as UNINITIALIZED, and the variable
   initialized from it inherits that.

   The distinction that matters is "no state yet" versus "known uninitialized",
   and the fix is at the ASSIGNMENT, not at the operator. Seeding the operator's
   result with a boolean ANY was tried first and reverted: compound-assign.c has
   a compile_assert that passes precisely because the result stays unseeded and a
   later, more precise mechanism supplies the value, and collapsing unseeded to
   ANY loses that.

   Instead, flow3_check_object_init_assigment used to RETURN when the source had
   no tracked state, leaving the destination at its declaration-time
   UNINITIALIZED. An assignment did happen, so the destination is now recorded
   as initialized-with-unknown-value (ANY) in that case. The operator keeps its
   existing precision when it has any.

   EXPR_LOGICAL_AND has the identical `else` branch, hence and_is_boolean below.
*/

#define NULL ((void*)0)

struct T { int type; };
struct L { struct T* _Opt head; };

/* The reduced tokenizer.c shape: right operand's value depends on the path. */
_Bool or_is_boolean(struct L r4)
{
    const _Bool e = (r4.head == NULL || r4.head->type == 1);
    return e;
}

/* Same for &&. */
_Bool and_is_boolean(struct L r4)
{
    const _Bool e = (r4.head != NULL && r4.head->type == 1);
    return e;
}

/*
   Control 1: a single comparison. Already clean -- one operand, no per-path
   seeding needed, so the result is never left unseeded.
*/
_Bool single_comparison_ok(struct L r4)
{
    const _Bool e = (r4.head == NULL);
    return e;
}

/*
   Control 2: the same logic spelled out with if/else. Already clean, because
   both arms assign a concrete value rather than relying on the operator's
   result being seeded.
*/
_Bool spelled_out_ok(struct L r4)
{
    _Bool e;
    if (r4.head == NULL)
        e = 1;
    else
        e = (r4.head->type == 1);
    return e;
}
