#pragma safety enable

/*
   The POINTEE of a `T**` parameter must honour its declared nullability.

   For a parameter `struct token** pp`, the inner pointer `*pp` has type
   `struct token*` (non-_Opt), so it is non-null on entry. flow3 used to seed
   `*pp` as ANY (possibly-null) because, when seeding a pointer parameter's
   pointed object, it recursed with a NULL type and lost the pointee's real
   type -- so the "non-_Opt pointer => non-null" rule never applied one level
   down. Passing `pp` on to another function whose pointee is non-nullable then
   produced a false:

       "passing a possible null pointer '(*pp)' to non-nullable pointer parameter"

   (Reproduced from parser.c:2184: parser_match_tk_lint forwarding
    pp_token_lint to parser_match_tk_core.)
*/

struct token { int x; };

/* callee: the T** itself is _Opt (may be null); its pointee is non-_Opt. */
static void core(struct token** _Opt pp);

/* Non-_Opt pointee: *pp is non-null -> forwarding pp is clean (was a false
   positive on '(*pp)'). */
void wrapper(struct token** pp)
{
    core(pp);
}

/* Non-_Opt pointee: dereferencing (*pp)->x is safe -> clean. */
int read_non_opt(struct token** pp)
{
    return (*pp)->x;
}

/*
   Safety is preserved for a genuinely nullable pointee (documented, since it
   warns): with `struct token* _Opt* pp`, *pp may be null, so

       int read_opt(struct token* _Opt* pp) { return (*pp)->x; }

   still reports "-> operator applied to a null pointer".
*/
