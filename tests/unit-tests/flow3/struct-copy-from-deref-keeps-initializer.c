#pragma safety enable

/*
   Whole-struct assignment from a dereferenced pointer does not update the
   destination's member state, so the destination keeps the values its
   initializer gave it and later tests on its members fold to constants.

       struct s2 out = {0};
       out = *list[j];
       if (out.k != 0) { break; }   // reported unreachable -- WRONG

   `*list[j]` can hold any value, so `out.k != 0` is plainly reachable.

   Proven with cake's own static_debug, inserted after the assignment in the
   original code (src/flow3.c, the EXPR_UNARY_NEG/PLUS mapping loop):

       note: '...value_kind' == 0 line 9519 "loop body (first pass)"

   Line 9519 is `struct flow3_alternative out = {0};` -- i.e. after
   `out = *list[j];` on the following line, flow3 still holds the value from
   the ZERO INITIALIZER. The copy contributed nothing.

   This is why it bites so hard in practice: the affected enum's first
   enumerator is 0 (FLOW3_VALUE_KIND_SIGNED = 0), so the stale initializer
   value reads as a legitimate enum value and the comparison folds silently
   rather than looking obviously wrong.

   Confirming asymmetry in the original code: the sibling branch writes the
   same variable THROUGH A POINTER instead --

       if (!flow3_alt_negate(list[j], &out)) { all_mapped = false; break; }

   -- and is NOT reported unreachable. Same variable, same enclosing loop,
   same `{ ...; break; }` body. Only the struct-assignment branch is folded,
   because passing &out invalidates flow3's state for it while the struct
   copy does not.

   FIXED, in exactly that direction: flow_check_object_init_assigment now
   detects a struct/union source whose members carry no state at all -- no
   member entry in the map, and no REF alternative resolving to an object that
   has one -- and sets the DESTINATION's members to ANY instead of walking the
   empty members and leaving the destination as it was.

   Arrays are deliberately excluded from that rule: a string-literal source
   has no member state either, but C zero-fills the elements the initializer
   does not supply and array-string-init-zero-fill.c depends on it.

   The `list` parameters carry `const` at both levels so the sample is clean
   with warning 82 enabled too, not merely under the suite's -wd82.
*/

struct s2 { int k; int v; };

void two_int_members(const struct s2* _Opt const* _Opt list, int count)
{
    for (int j = 0; j < count; j++)
    {
        struct s2 out = {0};
        out = *list[j];
        if (out.k != 0) { break; } /* reachable: *list[j] can hold anything */
    }
}

/* Same defect with a union member, matching the original struct's shape. */
struct s_union
{
    int value_kind;
    union { long long i; unsigned long long u; void* p; } value;
    int rel;
};

void with_union_member(const struct s_union* _Opt const* _Opt list, int count)
{
    for (int j = 0; j < count; j++)
    {
        struct s_union out = {0};
        out = *list[j];
        if (out.value_kind != 0) { break; } /* reachable */
    }
}
