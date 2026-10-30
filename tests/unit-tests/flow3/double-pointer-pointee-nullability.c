#pragma safety enable

/* for `struct token** pp` the pointee *pp is non-_Nullable and so non-null on entry; forwarding pp must not warn (parser.c parser_match_tk_lint) */

struct token { int x; };

/* callee: the T** itself is _Nullable (may be null); its pointee is non-_Nullable. */
extern void core(struct token** _Nullable pp);

/* non-_Nullable pointee: forwarding pp is clean */
void wrapper(struct token** pp)
{
    core(pp);
}

/* *pp is shared with the caller: reading it is the worst case, it may be null (flow.md 2.2) */
int read_non_opt(struct token* const* pp)
{
    return (*pp)->x; //lint 33
}

/* documented: with `struct token* _Nullable* pp`, (*pp)->x still warns */
