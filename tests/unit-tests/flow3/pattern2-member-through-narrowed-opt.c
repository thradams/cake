#pragma safety enable

/* FIXED: a non-_Nullable member read through a narrowed _Nullable pointer is non-null by contract (expressions.c:1017) */

#define NULL ((void*)0)

struct token { char* _Nonnull _Owner lexeme; };      /* lexeme is non-_Nullable -> non-null */
struct ctx   { struct token* current; };

/* Base pointer narrowed from _Nullable: now correctly clean. */
int through_narrowed_opt(const struct ctx* c)
{
    if (c->current == NULL)
        return 1;                            /* c->current now non-null */

    const char* s = c->current->lexeme;      /* non-_Nullable member -> non-null */
    while (*s)                               /* clean (was a false positive) */
        s++;
    return 0;
}

/* CONTRAST (clean): base pointer inherently non-null (non-_Nullable parameter). */
int ok(const struct token* t)
{
    const char* s = t->lexeme;
    while (*s)
        s++;
    return 0;
}
