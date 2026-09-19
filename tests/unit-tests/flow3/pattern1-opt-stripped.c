#pragma safety enable

/* NULL to a non-nullable parameter is flagged (60 + 35); if _Opt is #defined to empty the same call is wrongly flagged */

#define NULL ((void*)0)
struct marker { int z; };

/* Real _Opt keyword: a NULL argument is legal -> no warning. */
int diag_opt(const struct marker* _Opt m);
void ok(void)
{
    diag_opt(NULL);
}

/* _Opt stripped to empty: the identical NULL is flagged */
#define _Opt
int diag_stripped(const struct marker* _Opt m);
void bad(void)
{
    diag_stripped(NULL);          /* flagged: _Opt was stripped */ //lint 35 passing a possible null pointer '((void*)0)' to non-nullable pointer parameter (see line
}
