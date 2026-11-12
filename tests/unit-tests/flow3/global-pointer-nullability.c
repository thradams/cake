#pragma safety enable

/* a global pointer is nullable unless _Nonnull (flow.md 2.1): only a parameter is non-null by default */

#define NULL ((void*)0)

extern int* _Nonnull nonnull_global; /* _Nonnull    -> non-null */
extern int* opt_global;              /* nothing said -> possibly null */

void takes_nonnull(int* p);

/* _Nonnull global is non-null: no warning. */
void use_nonnull(void)
{
    takes_nonnull(nonnull_global);
    *nonnull_global = 1;
}

/* a global is possibly-null by default: must be guarded. */
void use_opt(void)
{
    if (opt_global != NULL)
        takes_nonnull(opt_global);   /* ok: guarded */
}

void use_opt_unguarded(void)
{
    takes_nonnull(opt_global); //lint 35
    *opt_global = 1;           //lint 33
}
