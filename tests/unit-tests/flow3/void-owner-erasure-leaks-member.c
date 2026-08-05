#pragma safety enable

/*
   Companion to void-owner-param-skips-member-checks.c.

   That fix made passing an _Owner pointer to a void* _Owner destination
   (a parameter like free()'s, an assignment, or a return) skip checking
   the pointee's member state, because void* can never be read back
   into to report on members whose lifetime already legitimately ended
   (e.g. via a prior _Dtor call). But the skip was unconditional: it
   also swallowed members that were simply never released, silently
   accepting a leak the same code would have flagged as "owner object
   not moved" had the struct gone out of scope normally instead of
   being erased to void*.

   struct X has two owned members: .name is freed, .surname is not. One
   correct free() call next to it is not enough to prove the whole
   struct is clean -- flow analysis checks every _Owner member
   individually before the type erasure to void* discards that
   information for good. Note .surname is never read anywhere in these
   functions -- that is the point: the member a caller forgot to
   release is, by definition, the one nothing in the code happens to
   touch, so the check must not depend on the code having read it
   first.
*/

struct X
{
    int* _Owner _Opt name;
    int* _Owner _Opt surname;
};

void free(void* _Owner _Opt p);

/* Erasure through a function parameter (e.g. free()). */
void delete1(struct X* _Owner _Opt p)
{
    if (p)
    {
        free(p->name);
        /* forgot to free p->surname */
        free(p); //lint 29 owner object (.surname) not moved (opt-mem-nonnull see line 40)
    }
}

/* Erasure through an assignment/initialization. */
void assign1(struct X* _Owner _Opt p)
{
    if (p)
    {
        free(p->name);
        /* forgot to free p->surname */
        void* _Owner _Opt v = p; //lint 29 owner object (.surname) not moved (opt-mem-nonnull see line 51)
        free(v); //lint 29 owner object (.surname) not moved (opt-mem-nonnull see line 51)
    }
}

/* Erasure through a return. */
void* _Owner _Opt detach1(struct X* _Owner _Opt p)
{
    if (p)
    {
        free(p->name);
        /* forgot to free p->surname */
        return p; //lint 29 owner object (.surname) not moved (opt-mem-nonnull see line 62)
    }
    return 0;
}
