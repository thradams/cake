#pragma safety enable

/*
   Advancing a pointer (p++, ++p, p = p + 1) must INVALIDATE what is known
   about its pointee: after the move it points at a different element whose
   value is unknown. Its null-ness is preserved (it still points within the
   same object), but a value fact like `*p == c` must not survive.

   flow3 kept the same pointee object across an increment ("advancing a
   pointer keeps its null/pointee identity"), so a fact established by a scan
   loop -- e.g. `while (*p != '"') p++;` leaves `*p == '"'` -- stayed attached
   to the advanced pointer. The next test `*p != '"'` then folded to false and
   its body was reported as unreachable code.

   Fix: on advance, repoint to a fresh ANY pointee. A pointer copied off
   BEFORE the increment keeps the old pointee, so `*q` stays known.

   (Reproduced from tokenizer.c decode_pragma_string.)
*/

/* Scan to the opening quote, step past it, then scan the body. The inner
   loop body must be reachable. */
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
