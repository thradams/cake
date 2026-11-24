#pragma safety enable

/* NULL to a non-nullable parameter is flagged (60 + 35); if _Nullable is #defined to empty the same call is wrongly flagged */

#define NULL ((void*)0)
struct marker { int z; };

/* Real _Nullable keyword: a NULL argument is legal -> no warning. */
int diag_opt(const struct marker* _Nullable m);
void ok(void)
{
    diag_opt(NULL);
}

/* _Nullable stripped to empty: the identical NULL is flagged */
#define _Nullable
int diag_stripped(const struct marker* _Nullable m);
void bad(void)
{
    diag_stripped(NULL);          /* flagged: _Nullable was stripped */ //lint 35 passing a possible null pointer '((void*)0)' to non-nullable pointer parameter (see line
}
