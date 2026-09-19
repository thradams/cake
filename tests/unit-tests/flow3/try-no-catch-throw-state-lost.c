#pragma safety enable

/* `try` without `catch`: a throw resumes after the try, so the throw-join state (p MOVED) must merge with the normal completion (p == 0) */

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

/* control: with a catch the throw path goes through the catch body, unchanged */
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

/* control: no throw in the body, the empty join must not be merged */
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
