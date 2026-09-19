#pragma safety enable

/* a `T**` out-parameter's inner nullability is invariant: forwarding pp to a callee that stores NULL through it needs `T* _Opt* _Opt` (parser.c find_declarator) */

#define NULL ((void*)0)

struct scope { int x; };
struct map_entry { int y; };

struct map_entry* _Opt find_variables(const char* lexeme, struct scope* _Opt* _Opt pp);

/* BAD: inner non-nullable, forwarded to a callee that writes NULL -> warns. */
struct map_entry* _Opt forward_bad(const char* lexeme, struct scope** _Opt pp)
{
    return find_variables(lexeme, pp); //lint 35 passing a possible null pointer '(*pp)' to non-nullable pointer parameter (see line 26)
}

/* GOOD: inner _Opt matches the callee -> clean. */
struct map_entry* _Opt forward_ok(const char* lexeme, struct scope* _Opt* _Opt pp)
{
    return find_variables(lexeme, pp);
}
