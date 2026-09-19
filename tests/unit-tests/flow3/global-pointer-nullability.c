#pragma safety enable

/* a global pointer respects its declared nullability: non-_Opt is non-null (stdout), _Opt is possibly null */

#define NULL ((void*)0)

extern int* non_opt_global;          /* non-_Opt -> non-null */
extern int* _Opt opt_global;         /* _Opt     -> possibly null */

void takes_nonnull(int* p);

/* Non-_Opt global is non-null: no warning. */
void use_non_opt(void)
{
    takes_nonnull(non_opt_global);
    *non_opt_global = 1;
}

/* _Opt global is possibly-null: must be guarded. */
void use_opt(void)
{
    if (opt_global != NULL)
        takes_nonnull(opt_global);   /* ok: guarded */
}

/* original case: `fputs(s, stdout)` is clean, stdout is non-null */
