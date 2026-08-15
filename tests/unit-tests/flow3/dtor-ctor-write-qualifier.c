#pragma safety enable

/*
   Regression tests for the _Dtor / _Out write-qualifiers, complementing
   clear-parameter.c (which covers _Clear) and ownership.c (which already
   has a couple of basic _Out/_Dtor exit-check cases: x_init/x_init_forgot,
   x_destroy/x_destroy_forgot).

   flow3_check_write_qualifier_parameters (flow3.c) rejects two kinds of
   misuse, the same way for all three of _Clear/_Dtor/_Out:

     1. Applied to a NON-pointer parameter -- error 1940 "must be used only
        at the pointed object". See the non-pointer rejection block below.

     2. Applied together with `const` on the pointee -- error 1930 "pointee
        cannot also be const" -- since both _Dtor and _Out mean the callee
        WRITES through the pointer (destroys it / initializes it), which
        contradicts const's promise not to modify it. Checked for both
        qualifier orders (`const _Dtor T*` and `_Dtor const T*`, same for
        _Out).

   Both checks were verified empirically against the built cake binary
   (not just by reading the code) for _Dtor and _Out specifically, since
   only _Clear had a directly-tested example previously:

     #pragma safety enable
     void f1(_Dtor int x) {}                       // error 1940
     void f2(_Out int x) {}                       // error 1940
     void f3(const _Dtor struct X* p) {}            // error 1930
     void f4(_Dtor const struct X* p) {}            // error 1930
     void f5(const _Out struct X* p) {}            // error 1930
     void f6(_Out const struct X* p) {}            // error 1930

   All six fired the expected error when tried directly. The commented-out
   block at the end of this file reproduces the same six cases so they can
   be re-verified by uncommenting.

   The rest of this file exercises the _Out/_Dtor EXIT-check semantics
   (flow3_check_clear_params_at_exit's counterpart logic for pointed_ctor/
   pointed_dtor, wired the same way as _Clear's): a _Out parameter's
   pointee must be a fully constructed value (not left uninitialized) by
   every exit point -- EVERY member, not just _Owner ones, the same
   "definite assignment" obligation C#'s `out` enforces for every field.
   A _Dtor parameter's pointee, by contrast, only obligates its _Owner
   members: they must have had their lifetime ended (freed/moved), not
   just their bits zeroed -- plain scalar members are the caller's business
   either way, so _Dtor has nothing to say about them.
*/

struct pair
{
    int a;
    int b;
};

void ctor_pair_ok(_Out struct pair* p)
{
    p->a = 1;
    p->b = 2;
} /* ok: every member initialized */

void ctor_pair_forgets_member(_Out struct pair* p)
{
    p->a = 1;
    /* p->b never written -- leak of definite assignment, not of a resource */
} //lint 71 _Out parameter 'p' pointee (.b) is possibly not initialized at exit (see line 63)

void ctor_pair_conditional(_Out struct pair* p, int flag)
{
    if (flag)
    {
        p->a = 1;
        p->b = 2;
    }
    else
    {
        p->a = 0;
        p->b = 0;
    }
} /* ok: both branches initialize both members */

void use_ctor_pair_ok(void)
{
    struct pair p;
    ctor_pair_ok(&p);
    /* At the CALL SITE, flow3 does not simulate the callee's body, so it
       cannot know the exact values ctor_pair_ok wrote -- it only knows
       (via type_is_pointed_ctor -> flow3_map_set_object_any) that *p is
       now a genuine, initialized value of unknown content. So asserting
       a specific value here correctly fails (uncomment to see error
       1080); what IS guaranteed is that reading p.a/p.b no longer warns
       about a possible uninitialized read. */
    // static_debug(p);
    // compile_assert(p.a == 1); // fails: any value, including zero
}

char* _Owner _Opt strdup(const char* s);
void free(void* _Owner _Opt p);

struct owned
{
    char* _Owner _Opt text;
};

void ctor_owned_ok(_Out struct owned* p)
{
    p->text = strdup("hi"); /* initializes the owned resource: this is what _Out requires */
} /* ok */

void ctor_owned_forgets_init(_Out struct owned* p)
{
    /* p->text never written -- the _Owner member is left uninitialized */
} //lint 71 _Out parameter 'p' pointee (.text) is possibly not initialized at exit (see line 112)

void dtor_owned_ok(_Dtor struct owned* p)
{
    free(p->text); /* releases the owned resource: this is what _Dtor requires */
} /* ok */

void dtor_owned_forgets_release(_Dtor struct owned* p)
{
    /* never frees/moves p->text -- resource leak on a _Dtor exit */
} //lint 29 owner object (.text) not moved (opt-nonnull see line 106)

/*
   FIXED (originally found while writing this file as a known gap, then
   root-caused after a second real-code report of the same shape):
   flow3_map_set_object_lifetime_ended correctly marked a _Dtor-consumed
   owner's alternatives ENDED (visible via static_debug below), but
   flow3_check_object_at_exit -- the "_Owner not moved" scope-exit check
   -- only ever recognized FLOW3_IMAGINARY_MOVED as "resolved", not
   ENDED, so it still reported a false "owner not moved" here and (worse)
   on plain non-_Opt struct members passed to a _Dtor function, e.g.
   `hash_item_set_destroy(&item);` after `hashmap_set(map, key, &item);`
   in real code. Fixed by accepting ENDED alongside MOVED in that check.
*/
void use_dtor_owned(void)
{
    struct owned o;
    o.text = strdup("hi");
    dtor_owned_ok(&o); /* ok: _Dtor call ends o.text's lifetime */
    // static_debug(o); /* shows o.text == ENDED on both forked alternatives */
}

/*
   Uncomment any line below to see the corresponding error fire. Left
   commented out so this file itself compiles cleanly end to end.

   struct X { int a; };

   error 1940 "_Dtor/_Out must be used only at the pointed object"
   (non-pointer parameter -- there is no "pointed object" to speak of):
   void dtor_on_non_pointer_rejected(_Dtor int x) {}
   void ctor_on_non_pointer_rejected(_Out int x) {}

   error 1930 "_Dtor/_Out pointee cannot also be const" (either order --
   _Dtor/_Out both mean the callee writes through the pointer, which
   contradicts const):
   void dtor_and_const_rejected_a(const _Dtor struct X* p) {}
   void dtor_and_const_rejected_b(_Dtor const struct X* p) {}
   void ctor_and_const_rejected_a(const _Out struct X* p) {}
   void ctor_and_const_rejected_b(_Out const struct X* p) {}
*/
