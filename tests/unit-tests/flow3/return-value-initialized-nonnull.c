#pragma safety enable

/*
   Two related default assumptions about a called function's return
   value:

     1. A returned struct/value is assumed fully initialized -- using
        its members right away doesn't warn about possibly-uninitialized
        reads.

     2. A returned pointer follows the same non-_Opt/_Opt rule as a
        parameter: non-_Opt means assumed non-null (safe to dereference
        immediately); _Opt means it may be null (must be checked first).
*/

struct X
{
    int a;
    int b;
};

struct X make(void);

void use_struct(void)
{
    struct X x = make();
    /* Note: reading x.a/x.b combined via `+` doesn't actually exercise
       the "assumed initialized" check below (that combination happens
       not to consult it at all -- a separate, narrower limitation) --
       see call-return-value-initialized.c for a DIRECT member read,
       which is what actually exercises (and, once fixed, confirms) the
       rule this comment describes. */
    int y = x.a + x.b; /* ok: return value assumed fully initialized */
}

struct X* make_nonnull(void);

void use_ptr_nonnull(void)
{
    struct X* p = make_nonnull();
    p->a = 1; /* ok: non-_Opt return pointer assumed non-null */
}

struct X* _Opt make_opt(void);

void use_ptr_opt_unguarded(void)
{
    struct X* _Opt p = make_opt();
    p->a = 1; //lint 33 -> operator applied to a null pointer
}

void use_ptr_opt_guarded(void)
{
    struct X* _Opt p = make_opt();
    if (p)
    {
        p->a = 1; /* ok: narrowed non-null */
    }
}
