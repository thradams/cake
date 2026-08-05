#pragma safety enable

/*
   `_Opt` has two distinct positions, exactly like `const`:

       struct X*      _Opt p    // _Opt binds to the POINTER: p may be null.
       _Opt struct X* p         // _Opt binds to the POINTEE (the struct-specifier):
                                 // p itself stays a guaranteed non-null pointer,
                                 // but what it points to is not guaranteed to be a
                                 // fully-formed object (the same spirit as a _Dtor
                                 // pointee accepting a partially-created one, see
                                 // samples/flow3/dtor_is_opt.c).

   So a member that is NOT itself declared _Opt, reached only through an
   `_Opt struct X*` parameter, must still be treated as possibly null: p can't
   be null, so there is never a warning about `p` itself, but `p->p_not_opt`
   is possibly null even though its own declared type says otherwise.

   flow3_parameter_object_init_r detects this (pointee_is_opt, computed from
   the pointee's own type qualifiers) and propagates force_opt into the
   pointee's members, the same way dest_is_dtor propagates permissiveness
   down the recursion in flow3_check_object_init_assigment.

   Contrast `through_plain` below: the same member, reached through a pointer
   whose pointee has no _Opt qualifier at all, stays governed by its own
   declared nullability and is correctly treated as non-null.
*/

struct X
{
    char* p_not_opt; /* not _Opt -- non-null by its own declared type */
};

void through_opt_pointee(_Opt struct X* p)
{
    /* p can't be null -- no warning here even without a guard. */
    *p->p_not_opt = 'a'; //lint 33 possible null pointer dereference
}

void through_plain(struct X* p)
{
    *p->p_not_opt = 'a'; // ok: pointee has no _Opt taint, member stays non-null by contract
}

/* NOTE: `_Dtor _Opt struct X* p` and `_Dtor struct X* p` are equivalent.
   A _Dtor pointee is already permissively modeled (every member may be null,
   see dtor_is_opt.c) regardless of pointee_is_opt, so writing an explicit
   `_Opt` alongside `_Dtor` adds nothing -- it's redundant, not a stronger or
   different contract. */
void destroy_x1(_Dtor _Opt struct X* p) { (void)p; }
void destroy_x2(_Dtor struct X* p) { (void)p; }

/* NOTE: `_View` has an analogous "taints members" behavior to _Opt, but for
   ownership rather than nullability: `_View struct S s` (or a `_View`
   pointee) makes every _Owner member of s a view -- reading/passing it never
   moves ownership, even though the member's own declared type still says
   _Owner. See flow3_check_object_init_assigment's `view_here` propagation
   (flow3.c) and samples/flow3/ownership.c. */
