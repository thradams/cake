#pragma safety enable

/*
   Regression tests for the _Clear parameter qualifier.

   `_Clear T* p` on a function parameter has two sides, both checked in
   flow3.c now:

   1. At the CALL SITE: flow3 must treat the argument's pointee
      (recursively, for every member of a struct) as being exactly 0
      after the call. This already worked correctly
      (flow3_visit_function_arguments -> flow3_check_object_init_assigment
      -> flow3_map_set_object_zero, gated on type_is_pointed_clear and
      INIT_PARAMETER); see whole_struct_cleared below.

   2. At the IMPLEMENTATION itself: flow3 now also checks, at every
      exit point of the function that DECLARES the _Clear parameter,
      that the pointee really was set to zero -- _Clear is similar to
      _Dtor in this respect (both describe a contract the callee must
      fulfill by the time it exits), except the contract is "every
      member is exactly 0" rather than "the pointee's lifetime ended".
      See clear_outer (correct) vs clear_outer_forgets_a_member and
      clear_outer_does_nothing (both flagged) below.

   Two other gaps were found and fixed while implementing this:

   - `const _Clear T* p` was silently accepted, in both qualifier
     orders. _Clear means the callee WRITES zero to everything *p
     points to, which directly contradicts const's promise not to
     modify it. Same reasoning applies to `const _Dtor T* p` and
     `const _Out T* p`. Rejected with error 1930; see
     clear_and_const_rejected below.

   - `_Clear`/`_Dtor`/`_Out` on a NON-pointer parameter was also
     silently accepted (e.g. `_Clear int x`), even though all three
     only make sense as a qualifier on what a pointer points to.
     Rejected with error 1940; see clear_on_non_pointer_rejected below.

   Both new checks live in flow3_check_write_qualifier_parameters, and
   the implementation check lives in flow3_check_clear_params_at_exit
   (called from flow3_check_function_exit for explicit returns, and
   from flow3_visit_declaration for falling off the end of the
   function) plus flow3_check_clear_object_is_zero_at_exit (the
   recursive per-member check, structurally mirroring the existing
   _Owner "not moved" check in flow3_check_object_at_exit).
*/

struct inner
{
    int a;
    int b;
};

struct outer
{
    struct inner in;
    int c;
};

void clear_outer(_Clear struct outer* p)
{
    p->in.a = 0;
    p->in.b = 0;
    p->c = 0;
}

void clear_outer_forgets_a_member(_Clear struct outer* p)
{
    p->in.a = 0;
    p->in.b = 0;
    /* forgot p->c = 0; */
} //lint 69 _Clear parameter 'p' pointee (.c) is never set to zero

void clear_outer_does_nothing(_Clear struct outer* p)
{
    /* does not touch *p at all -- flagged for every leaf member */
} //lint 69 69 69 _Clear parameter 'p' pointee (.in.a) is never set to zero; _Clear parameter 'p' pointee

void whole_struct_cleared(void)
{
    struct outer o = { { 1, 2 }, 3 };
    clear_outer(&o);
    /* expected: every member, including the nested struct's, is 0 */
    // static_debug(o);
    compile_assert(o.in.a == 0);
    compile_assert(o.in.b == 0);
    compile_assert(o.c == 0);
}

void without_the_call_values_are_unchanged(void)
{
    /* contrast: no _Clear call here, so the original values survive */
    struct outer o = { { 1, 2 }, 3 };
    compile_assert(o.in.a == 1);
    compile_assert(o.in.b == 2);
    compile_assert(o.c == 3);
}

/*
   Uncomment any line below to see the corresponding error fire. Left
   commented out so this file itself compiles cleanly end to end.

   error 1930 "_Clear pointee cannot also be const" (either order):
   void clear_and_const_rejected_a(const _Clear struct outer* p) {}
   void clear_and_const_rejected_b(_Clear const struct outer* p) {}

   error 1940 "_Clear must be used only at the pointed object":
   void clear_on_non_pointer_rejected(_Clear int x) {}
*/
