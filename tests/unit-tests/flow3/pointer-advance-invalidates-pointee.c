#pragma safety enable

/* advancing a pointer repoints it to a fresh ANY pointee: `*p == '"'` from a scan loop does not survive p++, a copy taken before keeps the old pointee (tokenizer.c decode_pragma_string) */

/* scan to the quote, step past it, scan the body: the inner loop body must be reachable */
unsigned long body_len(const char* literal)
{
    while (*literal != '"')
        literal++;              /* on exit: *literal == '"' */
    literal++;                  /* advance: *literal now unknown */

    unsigned long len = 0;
    const char* p = literal;
    while (*p && *p != '"')     /* body must NOT be unreachable */
    {
        len++;
        p++;
    }
    return len;
}

/* A copy taken BEFORE the advance keeps its own knowledge of the pointee. */
int copy_before_advance(const char* s)
{
    while (*s != '"')
        s++;                    /* *s == '"' */
    const char* q = s;          /* q aliases the quote position */
    s++;                        /* advancing s must not disturb q */
    if (*q == '"')
        return 1;               /* reachable: q still points at the quote */
    else
        return 0;               /* also reachable (q's value is a fact, not a proof of the else) */
}
