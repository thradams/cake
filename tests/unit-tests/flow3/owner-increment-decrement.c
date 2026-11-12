#pragma safety enable

/* ++ and -- on an _Owner pointer are errors in all four forms (1310/1320), including the prefix forms that used to be accepted */

void free(void* _Owner _Opt p);

/* documented: p++ / ++p are error 1310, p-- / --p are error 1320 */

/* A plain (non-owner) pointer is unaffected: walking it is normal C. */
unsigned long count_chars(const char* s)
{
    unsigned long n = 0;
    while (*s)
    {
        n++;
        s++;        /* ok: s is a view, not an owner */
    }
    return n;
}

/* A view copied off an owner may be walked freely. */
unsigned long count_via_view(char* _Owner _Opt owned)
{
    unsigned long n = 0;
    if (owned)
    {
        const char* view = owned;   /* view of the owned buffer */
        while (*view)
        {
            n++;
            view++;                 /* ok: advancing the view, not the owner */
        }
    }
    free(owned);
    return n;
}
