#pragma safety enable
#define NULL ((void*)0)

/*
   Regression test for a false positive reported directly:

     struct X { int* p_non_null; };
     struct X x = make();
     f(&x);                  // f(struct X*), plain mutable pointer
     int y = *x.p_non_null;  // "possible null pointer dereference" -- false positive

   Whenever flow3 invalidates a struct's members because they might
   have been changed through a plain mutable pointer or a _Out call
   (flow3_map_set_object_any / flow3_map_set_object_any_n), it replaced
   every leaf member's tracked value with a generic
   FLOW3_VALUE_KIND_SIGNED ANY alternative -- regardless of the
   member's own type. flow3_alternative_can_be_zero treats ANY as
   "could be zero", so a perfectly ordinary NON-optional pointer member
   (whose own type already guarantees it can never be null) started
   looking possibly-null the moment its containing struct was
   invalidated, even though nothing could have made it null.

   This exact mechanism also explains two now-fixed pre-existing
   failures in this suite: owner-type-118.c (a _Out call invalidating
   non-_Opt _Owner/pointer members, then failing compile_assert(...!=0)
   and warning on a later _Dtor call) and owner-resource-122.c (same shape,
   simpler).

   Fixed by having flow3_map_set_object_any_n special-case pointer-typed
   members: a non-optional (non-_Opt) pointer member is seeded with a
   NOT_EQUAL-0 PTR alternative (unknown identity, but definitely
   non-null) instead of a plain ANY value -- the same guarantee flow3
   already gives a non-_Opt pointer PARAMETER on entry. An _Opt pointer
   member is unaffected and still correctly becomes possibly-null.
*/

struct X
{
    int* _Opt p_opt;
    int* p_non_null;
};

struct X make(void);
void f(struct X* p);

void use(void)
{
    struct X x = make();
    f(&x); /* plain mutable pointer: invalidates x's members */
    int y = *x.p_non_null; /* ok: non-optional pointer member stays non-null */
}

/* Contrast: an _Opt pointer member is still correctly possibly-null
   after the same invalidation. */
void use_opt_still_warns(void)
{
    struct X x = make();
    f(&x);
    int y = *x.p_opt; //lint 33 possible null pointer dereference
}
