#pragma safety enable

/*
   Regression test: once `if (p)` proves p is non-null, a later
   dereference of p inside that true branch must NOT warn -- the
   narrowing from the condition has to actually reach the dereference.

   Contrast the two functions below: same shape (p = something(); then
   a dereference of p), the only difference is the `if (p)` guard.
*/

struct X
{
    struct X* _Opt next;
};

struct X* _Opt something();

void guarded(struct X* _Opt p)
{
    p = something();
    if (p)
    {
        /* p narrowed to non-null by the if -- no warning here */
        p = p->next;
    }
}

void unguarded(struct X* _Opt p)
{
    p = something();
    p = p->next; //lint 33 -> operator applied to a null pointer
}
