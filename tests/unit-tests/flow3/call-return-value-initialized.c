#pragma safety enable

/* a call returning a non-pointer (scalar or struct) seeds its result as initialized, so `int e = mkdir(...)` / `struct X x = f()` are not uninitialized */

int mkdir(const char* path, int mode);

int copy_folder(const char* from, const char* to)
{
    int errcode = mkdir(to, 0700);
    if (errcode != 0)
    {
        return errcode; /* ok: errcode was assigned from mkdir's return value */
    }
    return 0;
}

struct X
{
    int a;
    int b;
    int* p;
};

struct X f(void);

int use_declaration_form(void)
{
    struct X x = f();
    int* q = x.p;
    return x.a + x.b + *q; /* ok: members of a returned struct are initialized, non-optional pointer members non-null */
}

int use_assignment_form(void)
{
    struct X x;
    x = f();
    int* q = x.p;
    return x.a + x.b + *q; /* ok: same for plain assignment */
}

/* contrast: a real uninitialized read is still flagged */
int real_uninitialized(void)
{
    struct X x;
    return x.a; //lint 30 passing a possible uninitialized object 'x.a' (see line 95)
}
