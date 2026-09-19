#pragma safety enable

/* _Dtor/_Out: rejected on a non-pointer (1940) and with a const pointee (1930); at exit a _Out pointee must be fully initialized, a _Dtor pointee's _Owner members must have ended */

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
    /* the call site only knows *p is initialized with unknown content, so a specific value is not provable but reading p.a/p.b no longer warns */
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

/* FIXED: the scope-exit "owner not moved" check accepts ENDED alongside MOVED (hash_item_set_destroy(&item) shape) */
void use_dtor_owned(void)
{
    struct owned o;
    o.text = strdup("hi");
    dtor_owned_ok(&o); /* ok: _Dtor call ends o.text's lifetime */
    // static_debug(o); /* shows o.text == ENDED on both forked alternatives */
}

/* uncomment to see error 1940 (_Dtor/_Out on a non-pointer) and error 1930 (const pointee) */
