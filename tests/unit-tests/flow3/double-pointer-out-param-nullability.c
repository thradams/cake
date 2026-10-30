#pragma safety enable

/* a `T**` out-parameter's inner nullability is invariant: forwarding pp to a callee that stores NULL through it needs `T* _Nullable* _Nullable` (parser.c find_declarator) */

#define NULL ((void*)0)

struct scope { int x; };
struct map_entry { int y; };

struct map_entry* find_variables(const char* lexeme, struct scope* _Nullable* _Nullable pp);

/* BAD: inner non-nullable, forwarded to a callee that writes NULL -> warns. */
struct map_entry* forward_bad(const char* lexeme, struct scope* _Nonnull* _Nullable pp)
{
    return find_variables(lexeme, pp); //lint 54 35
}

/* GOOD: inner _Nullable matches the callee -> clean. */
struct map_entry* forward_ok(const char* lexeme, struct scope* _Nullable* _Nullable pp)
{
    return find_variables(lexeme, pp);
}
