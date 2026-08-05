#pragma safety enable

/*
   p = &x takes the address of a block-scoped local. Once the enclosing
   block ends, x's storage duration is over and p is left dangling --
   flagged here as "lifetime has ended", the same diagnostic id (31)
   already used for a freed/_Dtor'd owner, since both are the same
   underlying fact: the object p points at no longer exists.

   Root cause found while investigating this: flow3_map_set_object_lifetime_ended
   only recursed into an aggregate's MEMBERS and marked those ENDED,
   never the aggregate object's own map entry. That was fine for the
   consumer that already existed (the owner "not moved" check walks
   members one by one anyway), but the arrow-dereference lifetime check
   (`p->i`, flow3_visit_expression's EXPR_POSTFIX_ARROW case) looks up
   the POINTEE's own top-level entry -- for `p = &x`, that pointee is
   x itself, not x.i -- so it never saw the ENDED state. Fixed by also
   marking the object's own entry before recursing into its members.
*/

struct X
{
    int i;
};

/* Escapes through one branch only; the other leaves p at its initial
   value (0), so p->i is unconditionally bad either way -- dangling if
   condition was true, null if it was false. */
void f(int condition)
{
   struct X * _Opt p = 0;
   if (condition)
   {
     struct X x = {};
     p = &x;
   }
   p->i = 1; //lint 31 -> operator: pointed object lifetime has ended
}

/* Escapes through BOTH branches -- p is never null here, only dangling. */
void both_branches_escape(int condition)
{
    struct X* _Opt p = 0;
    if (condition)
    {
        struct X x = {};
        p = &x;
    }
    else
    {
        struct X x2 = {};
        p = &x2;
    }
    p->i = 1; //lint 31 operator -> applied to 'p->i': pointed object lifetime has ended
}

/* Contrast: no escape past the local's own scope -- ordinary and safe. */
void no_escape(void)
{
    struct X x = {};
    struct X* p = &x;
    p->i = 1;
}
