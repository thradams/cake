#pragma safety enable

/*
   Nullability of a pointer-to-pointer out-parameter is INVARIANT: the inner
   pointee nullability must match between caller and callee, because the callee
   both reads and writes through it.

   find_variables writes `*pp = NULL`, so its inner pointer is _Opt (nullable).
   A forwarder that declares the inner as NON-nullable (`struct scope**`) but
   passes pp straight through is unsound -- the callee stores NULL into a slot
   the forwarder's signature promises is non-null. flow3 flags it (warning 35).

   Fix: declare the forwarder's inner pointer _Opt too, matching the callee
   (`struct scope* _Opt* _Opt`). Reduced from parser.c find_declarator /
   find_enumerator forwarding to find_variables.
*/

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
