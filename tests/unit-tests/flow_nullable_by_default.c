#pragma safety enable

// only a parameter is non-null by default. A file scope variable, a static
// local, an auto local, a member and a result are nullable when nothing is
// said, and _Nonnull says the opposite.

int* g;
int* _Nonnull g_nonnull;
int* _Nullable g_redundant; //lint 122
static int* s;
extern int* e;

int* opt(void);
int* _Nonnull never_null(void);

void file_scope(void)
{
    *g = 1; //lint 33
    *g_nonnull = 1;
    *s = 1; //lint 33
    *e = 1; //lint 33
}

void file_scope_tested(void)
{
    if (g)
        *g = 1;
}

void static_local(void)
{
    static int* p;
    *p = 1; //lint 33
}

void auto_local(void)
{
    auto p = opt(); /* ok: an auto local is nullable too */
    *p = 1; //lint 33

    auto q = never_null();
    *q = 1;
    q = 0; /* ok: q is a nullable local */
}

void parameter(int* p)
{
    *p = 1; /* ok: the only non-null default */
}
