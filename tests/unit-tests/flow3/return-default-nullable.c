#pragma safety enable
#define NULL ((void*)0)

/* an unannotated pointer return is nullable; _Nonnull makes it non-null */

int* f(void);
int* _Nonnull g(void);

void use_unannotated(void)
{
    int* p = f();
    int y = *p; //lint 33 possible null pointer dereference
}

void use_unannotated_checked(void)
{
    int* p = f();
    if (p)
    {
        int y = *p; /* ok: checked */
    }
}

void use_nonnull(void)
{
    int* p = g();
    int y = *p; /* ok: _Nonnull result */
}

int* return_null(void)
{
    return NULL; /* ok: unannotated return is nullable */
}

int* _Nonnull return_null_nonnull(void)
{
    return NULL; //lint 35
}
