#pragma safety enable

/* FIXED: a non-_Opt member read through a narrowed _Opt pointer is non-null by contract (expressions.c:1017) */

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
