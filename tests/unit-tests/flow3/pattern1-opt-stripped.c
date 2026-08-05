#pragma safety enable

/*
   _Opt on a pointer parameter is what makes a NULL argument legal. Passing
   NULL to a NON-nullable pointer parameter is flagged (warning 60 + warning
   35). The two functions below show both sides self-contained.

   (If `_Opt` is ever #defined to empty -- e.g. an unannotated header shadowing
   the keyword -- the parameter looks non-nullable and the identical NULL call
   is wrongly flagged. `bad()` reproduces that by stripping _Opt locally.)
*/

#define NULL ((void*)0)
struct marker { int z; };

/* Real _Opt keyword: a NULL argument is legal -> no warning. */
int diag_opt(const struct marker* _Opt m);
void ok(void)
{
    diag_opt(NULL);
}

/* _Opt stripped to empty: the parameter now looks non-nullable, so the
   identical NULL is flagged (warning 60 + warning 35). */
#define _Opt
int diag_stripped(const struct marker* _Opt m);
void bad(void)
{
    diag_stripped(NULL);          /* flagged: _Opt was stripped */ //lint 35 passing a possible null pointer '((void*)0)' to non-nullable pointer parameter (see line
}
