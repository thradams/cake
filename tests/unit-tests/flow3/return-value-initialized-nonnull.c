#pragma safety enable

/* a returned struct is assumed fully initialized; a returned pointer follows the non-_Opt/_Opt rule like a parameter */

struct X
{
    int a;
    int b;
};

struct X make(void);

void use_struct(void)
{
    struct X x = make();
    /* note: `x.a + x.b` does not exercise the initialized check, see call-return-value-initialized.c for a direct member read */
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
