#pragma safety enable
#define NULL ((void*)0)

/* invalidating a struct through a mutable pointer or _Out call keeps a non-_Opt pointer member non-null, only its identity is unknown */

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

/* contrast: an _Opt pointer member is possibly null after the same invalidation */
void use_opt_still_warns(void)
{
    struct X x = make();
    f(&x);
    int y = *x.p_opt; //lint 33 possible null pointer dereference
}
