#pragma safety enable

/* for `struct token** pp` the pointee *pp is non-_Opt and so non-null on entry; forwarding pp must not warn (parser.c parser_match_tk_lint) */

struct token { int x; };

/* callee: the T** itself is _Opt (may be null); its pointee is non-_Opt. */
extern void core(struct token** _Opt pp);

/* non-_Opt pointee: forwarding pp is clean */
void wrapper(struct token** pp)
{
    core(pp);
}

/* Non-_Opt pointee: dereferencing (*pp)->x is safe -> clean. */
int read_non_opt(struct token** pp)
{
    return (*pp)->x;
}

/* documented: with `struct token* _Opt* pp`, (*pp)->x still warns */
