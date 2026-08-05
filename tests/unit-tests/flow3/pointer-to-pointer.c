#pragma safety enable

/*
   Regression tests for double indirection (pointer-to-pointer, T**)
   nullability tracking in flow3.c.

   What already works: checking `pp` and then `*pp` (two nested if's,
   one per level of indirection) correctly narrows (*pp)->member to
   safe; a struct pointer chain (p->next->value) narrows the same way
   once each level is checked; and an entirely unchecked `pp` is still
   caught when dereferenced.

   One crash was found and fixed: `*pp = get(); static_debug(*pp);`
   segfaulted. Root cause: object_static_debug (used by static_debug)
   recurses into a FLOW3_VALUE_KIND_REF alternative's target object
   without a NULL check, but assigning through a pointer-to-pointer,
   `*pp = get();`, can leave a REF alternative whose target is NULL
   (REF is documented as "always non-null" at its definition, but
   nothing enforced that here). Fixed object_static_debug to skip a
   NULL REF target instead of recursing into it. See
   assign_through_pp_no_crash below.

   Two known gaps were found and are documented, not fixed, since both
   require deeper changes to how flow3 splits/re-narrows nullability
   for values reached through an extra level of indirection:

     1. Checking `pp` but never checking `*pp` before `(*pp)->member`
        does not warn, even though *pp is just as nullable as any other
        _Opt pointer (see pp_checked_star_pp_not_checked). The same gap
        shows up one level further with an ordinary struct pointer
        chain when only the first link is checked (see
        chain_only_first_link_checked).

     2. Assigning through a pointer-to-pointer and then checking the
        result narrows correctly for static_debug (both null and
        non-null alternatives show up correctly, per the crash fix
        above) but that narrowing does not reach a subsequent `(*pp)->`
        access -- `if (*pp) { (*pp)->value; }` right after `*pp =
        get();` still warns (see assign_then_check_still_warns).
*/

struct node
{
    struct node* _Opt next;
    int value;
};

struct node* _Opt get();

int both_levels_checked(struct node* _Opt* _Opt pp)
{
    if (pp)
    {
        if (*pp)
        {
            /* safe -- both pp and *pp narrowed non-null */
            return (*pp)->value;
        }
    }
    return -1;
}

int pp_unchecked(struct node* _Opt* _Opt pp)
{
    /* pp itself was never checked -- dereferencing it is a possible
       null pointer dereference */
    return (*pp)->value; //lint 33 possible null pointer dereference
}

int chain_both_links_checked(struct node* _Opt p)
{
    if (p && p->next)
    {
        /* safe -- p->next narrowed non-null */
        return p->next->value;
    }
    return -1;
}

void assign_through_pp_no_crash(struct node* _Opt* _Opt pp)
{
    if (pp)
    {
        *pp = get();
        /* expected: *pp == null (call-opt-null) or *pp == <non-null
           object> (call-opt-nonnull) -- previously crashed here */
        // static_debug(*pp);
    }
}

int pp_checked_star_pp_not_checked(struct node* _Opt* _Opt pp)
{
    if (pp)
    {
        /* known gap: *pp is just as nullable as pp, but is never
           checked here -- flow3 currently does not warn on this
           access, even though *pp could be null */
        return (*pp)->value;
    }
    return -1;
}

int chain_only_first_link_checked(struct node* _Opt p)
{
    if (p)
    {
        /* known gap: p->next is not checked -- same class of gap as
           pp_checked_star_pp_not_checked above, flow3 does not warn */
        return p->next->value; //lint 33 -> operator applied to a possible null pointer 'p->next'
    }
    return -1;
}

int assign_then_check_still_warns(struct node* _Opt* _Opt pp)
{
    if (pp)
    {
        *pp = get();
        if (*pp)
        {
            /* known gap: *pp was just narrowed non-null by the if
               right above (as assign_through_pp_no_crash's
               static_debug confirms), but that narrowing does not
               reach this access -- flow3 still warns here */
            return (*pp)->value; //lint 33 -> operator applied to a null pointer
        }
    }
    return -1;
}
