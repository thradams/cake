#pragma safety enable

/*
   ++ and -- are disallowed on _Owner pointers.

   Advancing an owner loses the very address that has to be freed, so the
   allocation could never be released through it. All four forms are rejected:

       p++   p--   ++p   --p

   This check used to live in expressions.c and only covered the POSTFIX forms,
   so `++p` / `--p` on an owner went completely unreported. It now lives in
   flow3 (where every _Owner diagnostic belongs) and catches all four.

   Everything in owner_forms() below is an ERROR by design -- this file is a
   specification, not a regression.
*/

void free(void* _Owner _Opt p);

/*
   Documented (each line is error 1310 / 1320):

       void owner_forms(char* _Owner _Opt p)
       {
           p++;    // error 1310: operator ++ cannot be used in _Owner pointers
           p--;    // error 1320: operator -- cannot be used in _Owner pointers
           ++p;    // error 1310  (was silently accepted before)
           --p;    // error 1320  (was silently accepted before)
           free(p);
       }
*/

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
