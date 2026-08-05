#pragma safety enable

/*
   PATTERN 2 (was a flow3 false positive, now FIXED) -- reading a NON-_Opt
   member through a pointer that was NARROWED from _Opt.

   After `if (p == NULL) return;` the pointer p is non-null. The pointee of a
   narrowed _Opt pointer was never member-seeded, so a non-_Opt (non-null)
   member read through it came back possibly-null and a later dereference
   warned. Fix: on a `->` member read, an unseeded member falls back to its
   declared nullability -- a non-_Opt pointer member is non-null by contract
   (flow3_seed_member_default). Both functions below are now clean.

   Reproduced from expressions.c:1017 (`while (*s)` with `s = token->lexeme`,
   token = ctx->current, an _Opt member guarded by an earlier NULL check).
*/

#define NULL ((void*)0)

struct token { char* _Owner lexeme; };      /* lexeme is non-_Opt -> non-null */
struct ctx   { struct token* _Opt current; };

/* Base pointer narrowed from _Opt: now correctly clean. */
int through_narrowed_opt(struct ctx* c)
{
    if (c->current == NULL)
        return 1;                            /* c->current now non-null */

    const char* s = c->current->lexeme;      /* non-_Opt member -> non-null */
    while (*s)                               /* clean (was a false positive) */
        s++;
    return 0;
}

/* CONTRAST (clean): base pointer inherently non-null (non-_Opt parameter). */
int ok(struct token* t)
{
    const char* s = t->lexeme;
    while (*s)
        s++;
    return 0;
}
