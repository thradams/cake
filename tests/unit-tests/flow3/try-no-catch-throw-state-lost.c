#pragma safety enable

/*
   `try` without a `catch` is legal -- `throw` still exits the try body
   immediately, it just has nowhere of its own to land, so control resumes
   right after the try statement (same as if the try body had ended
   normally at that point).

   flow3_visit_try_statement's "no catch block" branch only merged the
   try body's NORMAL-COMPLETION state (p_try_branch) into what comes after
   -- it silently dropped whatever state a `throw` left behind
   (p_throw_join, the map every `throw` inside the try accumulates into),
   as if throw never happened at all.

   User-reported repro:

       int * _Owner _Opt p = 0;
       try
       {
           int * _Owner _Opt p2 = p;   // moves p into p2
           if (condition) throw;       // exits here with p MOVED
           p = 0;                      // only reached if no throw
       }
       // no catch
       static_debug(p);   // showed only "== 0" -- missing "or MOVED"

   Fixed by merging p_throw_join in as a second arm (when it actually
   collected anything -- flow3_map_arm_has_entries guards against merging
   an empty join when no throw occurred, same guard already used for a
   loop's break-join at loop exit).
*/

int reported_repro(int condition)
{
    int* _Owner _Opt p = 0;
    try
    {
        int* _Owner _Opt p2 = p;
        if (condition) throw;
        p = 0;
    }
    /* no catch: throw exits the try body, control resumes here either way */
    static_debug(p); /* both '== 0' (fell through) and 'MOVED' (threw) must show */
    return 0;
}

/* Control: WITH a catch, the throw path already went through the catch
   body and its outcome is folded in via the catch branch -- must keep
   working exactly as before this fix (this path wasn't touched). */
int with_catch_unaffected(int condition)
{
    int* _Owner _Opt p = 0;
    try
    {
        int* _Owner _Opt p2 = p;
        if (condition) throw;
        p = 0;
    }
    catch
    {
        p = 0;
    }
    static_debug(p); /* always '== 0' here -- both arms set it */
    return 0;
}

/* Control: no throw anywhere in the try body -- the throw-join map is
   never populated, so flow3_map_arm_has_entries must keep it out of the
   merge (nothing to add, and merging an empty/untouched join must not
   itself introduce spurious alternatives). */
int no_throw_at_all(void)
{
    int* _Owner _Opt p = 0;
    try
    {
        p = 0;
    }
    static_debug(p); /* '== 0', single alternative */
    return 0;
}
