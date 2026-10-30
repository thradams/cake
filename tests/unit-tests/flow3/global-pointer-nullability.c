#pragma safety enable

/* a global pointer respects its declared nullability: non-_Nullable is non-null (stdout), _Nullable is possibly null */

#define NULL ((void*)0)

extern int* non_opt_global;          /* non-_Nullable -> non-null */
extern int* _Nullable opt_global;         /* _Nullable     -> possibly null */

void takes_nonnull(int* p);

/* Non-_Nullable global is non-null: no warning. */
void use_non_opt(void)
{
    takes_nonnull(non_opt_global);
    *non_opt_global = 1;
}

/* _Nullable global is possibly-null: must be guarded. */
void use_opt(void)
{
    if (opt_global != NULL)
        takes_nonnull(opt_global);   /* ok: guarded */
}

/* original case: `fputs(s, stdout)` is clean, stdout is non-null */
