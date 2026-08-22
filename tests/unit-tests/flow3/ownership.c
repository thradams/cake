



#pragma safety enable

void* _Owner _Opt _Uninitialized malloc(unsigned long size);
void free(void* _Owner _Opt ptr);
char* _Owner _Opt strdup(const char* s);

struct X {
    char* _Owner _Opt text;
    int i;
};

void x_init(_Out struct X* p);
void x_destroy(_Dtor struct X* p);

void test_nonnull_param(int* p)
{
    *p = 42; /* ok */
}

void test_nonnull_param_caller(int* _Opt q)
{
    test_nonnull_param(q); //lint 35 q may be null
}

void test_nonnull_param_caller_fixed(int* _Opt q)
{
    if (q)
    {
        test_nonnull_param(q); /* ok */
    }
}

int* test_nonnull_param_return(int* p)
{
    return p; /* ok */
}


void test_opt_param_guarded(int* _Opt p)
{
    if (p)
    {
        *p = 1; /*ok*/
    }
}

void test_opt_param_unguarded(int* _Opt p)
{
    *p = 1; //lint 33 p may be null
}

void test_opt_param_to_opt(int* _Opt p, int* _Opt* _Opt out)
{
    if (out) *out = p; /* ok */
}


void x_init(_Out struct X* p)
{
    p->text = strdup("hello");
    p->i = 0;
} /* ok: every member initialized */

void x_init_forgot(_Out struct X* p)
{
    /* p->text and p->i are never written */
} //lint 71 71 _Out parameter 'p' pointee (.text) and (.i) possibly not initialized at exit

int x_init_or_fail(_Out struct X* p, int flag)
{
    if (flag)
    {
        p->text = strdup("ok");
        p->i = 1;
        return 1;
    }
    p->text = 0;
    p->i = 0;
    return 0;
} /* ok: every member initialized on every path */

void x_destroy(_Dtor struct X* p)
{
    free(p->text);
} /* ok */

void x_destroy_forgot(_Dtor struct X* p)
{
    /* p->text is never freed/moved */
} //lint 29 owner object (.text) not moved (opt-nonnull see line 88)

void x_destroy_cond(_Dtor struct X* p)
{
    if (p->text)
        free(p->text); /* moved */
} /* ok */

void consume_x(struct X x)
{
    free(x.text);
}

void leak_x(struct X x) // owner object (.text) not moved (opt-nonnull see line 104)
{
    /* x.text is never freed or moved */
} //lint 29 owner object (.text) not moved (opt-nonnull see line 104)

void take_x(struct X x); /* forward */

void free_x(struct X* _Owner _Opt p)
{
    if (p)
    {
        x_destroy(p); /* moves *p contents */
        free(p);   /* frees p itself → p is expired */ // object '(*p).text' lifetime has ended (see line 115)
    }
} /* ok */

void free_x_forgot(struct X* _Owner p) // owner object (p) not moved (root see line 120)
{
    x_destroy(p); /* moves *p contents, but p itself still lives */
    /* missing: free(p) */
} //lint 29 owner object (p) not moved (root see line 120)

void read_x(_View struct X x) // ok: _View strips _Owner from members (.text is a view)
{

} /* ok */

void maybe_free(struct X* _Owner _Opt p)
{
    if (p)
    {
        x_destroy(p);
        free(p); // object '(*p).text' lifetime has ended (see line 135)
    }
    /* if p is null: nothing to free */
} /* ok */

void accepts_opt(int* _Opt p);

void test_nonnull_to_opt(int* p)
{
    accepts_opt(p); // ok
}

int* _Owner _Opt f(int c)
{
    int* _Owner _Opt p = malloc(sizeof * p);
    try
    {
        if (c)
            throw;
    }
    catch {
        free(p);
        p = nullptr;
    }

    return p; // object '(*p)' lifetime has ended (see line 157)
} /* ok */

void test_assign_owner_leak(void)
{
    struct X  a = { .text = strdup("hello") }; // owner object (.text) not moved (root see line 167)
    struct X  b = { .text = strdup("world") };
    a = b; //lint 26 expected warning
} //lint 29 owner object (.text) not moved (call-opt-nonnull see line 167)

int* _Opt test_return_owner_as_nonowner(void)
{
    int* _Owner _Opt p = malloc(sizeof(int)); // owner object (p) not moved (call-opt-nonnull see line 173)
    return p; //lint 29 owner object (p) not moved (call-opt-nonnull see line 173)
}

void* _Owner _Opt test_void_owner_bad(void)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    if (p)
        *p = 42;  // p is now initialized
    return p;
}

void test_void_owner_assign_bad(void)
{
    int* _Owner _Opt p = malloc(sizeof(int));
    if (p == nullptr)
        return;
    void* _Owner _Opt q = p;
    free(q);
}

struct Y { int x; };
void clear_y(_Clear struct Y* p);


void test_opt_to_nonnull_ambiguous(int* _Opt p)
{
    // p may be null or non-null; assigning to non-null pointer is invalid.
    int* q = p; //lint 35 p may be null
}

void takes_view(_View struct X x);

void test_pass_owner_to_view(void)
{
    struct X x = { .text = strdup("hello") }; // owner object (.text) not moved (root see line 208)
    takes_view(x); // passing a possible uninitialized object  '.text' (see line 208)
} //lint 29 owner object (.text) not moved (call-opt-nonnull see line 208)

int* f2();

void test_free_nonowner(void)
{
    int* p = f2();
    free(p); //lint 78 cannot assign a non-owner to owner
}
int* _Owner test_return_moved_bad(void)
{
    int* _Owner p = malloc(sizeof(int)); //lint 35 passing a possible null pointer 'malloc(sizeof(int))' to non-nullable pointer parameter
    free(p);
    return p; //lint 31 32 35 object 'p' is moved (see line 221)
}
struct X make(void)
{
    char* _Owner _Opt s = strdup("hello");
    return (struct X) { .text = s }; 
} // ok

void test_init_nonopt_null_bad(void)
{
    int* p = 0; //lint 35 Should error if nullable_enabled is on
}

void test_init_opt_null_ok(void)
{
    int* _Opt p = 0; // Should be OK
}

void test_pass_moved_bad(void)
{
    char* _Owner _Opt s = strdup("hi");
    free(s);
    accepts_opt(s); //lint 31 54 Should error: s is expired/moved
}

void take_non_owner(int* p);
void test_pass_owner_to_nonowner_bad(void)
{
    int* _Owner p = malloc(sizeof(int)); //lint 35 passing a possible null pointer 'malloc(sizeof(int))' to non-nullable pointer parameter
    take_non_owner(p); //lint 35 30 passing a possible null pointer 'p' to non-nullable pointer parameter (see line 251)
} //lint 29 owner object (p) not moved (call-opt-nonnull see line 251)
void test_clear_usage(void)
{
    struct Y y = { .x = 10 };
    clear_y(&y); // What is the expected state after this? No sample verifies it.
    compile_assert(y.x == 0);
}
