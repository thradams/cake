#pragma safety enable

/*
   Passing an array to a function whose parameter is a non-const ARRAY
   parameter must invalidate what the caller knows about the array's
   contents: the callee can write through it, exactly as it can through a
   pointer parameter.

   Without that write-effect the caller keeps the `= { 0 }` initializer as
   a live fact, folds `s[0] == 'F'` to always-false, and reports the body
   of the if as unreachable code even though the callee may well have
   stored 'F' there.

   The equivalent POINTER parameter (fill_ptr below) already applies the
   effect and is correct; only the array-parameter spelling is affected,
   for both a declared-only and a defined callee. Since `char s[4]` and
   `char *s` name the same parameter after C's adjustment rule, the three
   must analyse identically.

   Found in cake's own source: expressions.c reads a numeric suffix via

       char suffix[4] = { 0 };
       parse_number(buffer, suffix, errormsg);   // char suffix[4] parameter
       ...
       if (suffix[0] == 'F') { ... }             // reported unreachable

   _Ctor is NOT the right annotation for that parameter: floating_suffix_opt
   and integer_suffix_opt write it only conditionally and rely on the
   caller's zero-init for the no-suffix case, so it is a partially-written
   out-parameter, not a constructed one.

   flow3.c carries a note on the same root cause (the stale-seed comment
   citing parser.c:2064), worked around there for the negation path only.

   See also array_out.c, which covers this shape but leaves its `return 1;`
   commented out and so never asserts it.

   The //lint markers on A and B record today's wrong answer so the suite
   stays green. An unmatched marker fails the suite, so fixing the analyser
   will fail this test and prompt deleting them -- and C and D guard against
   "fixing" it by going silent, or noisy, everywhere.
*/

/* A: callee only declared */
void fill_decl(char s[4]);

int declared_callee(void)
{
    char s[4] = { 0 };
    fill_decl(s);
    if (s[0] == 'F')
    {
        return 1; //lint 68 KNOWN FALSE POSITIVE: reachable, fill_decl can have written s[0]
    }
    return 0;
}

/* B: callee defined in this TU, array parameter */
static void fill_def(char s[4])
{
    if (s[1] == 'x')
        s[0] = 'F';
}

int defined_callee(void)
{
    char s[4] = { 0 };
    fill_def(s);
    if (s[0] == 'F')
    {
        return 1; //lint 68 KNOWN FALSE POSITIVE: reachable, fill_def can have written s[0]
    }
    return 0;
}

/* C: same callee spelled with a pointer parameter -- already correct */
static void fill_ptr(char* s)
{
    if (s[1] == 'x')
        s[0] = 'F';
}

int pointer_parameter(void)
{
    char s[4] = { 0 };
    fill_ptr(s);
    if (s[0] == 'F')
    {
        return 1; /* ok: reachable, and correctly not reported */
    }
    return 0;
}

/* D: a const array parameter must NOT invalidate -- the callee cannot write
   it, so folding is right here and the body really is unreachable. */
static void read_only(const char s[4])
{
    if (s[0] == 'z')
        return;
}

int const_parameter_keeps_knowledge(void)
{
    char s[4] = { 0 };
    read_only(s);
    if (s[0] == 'F')
    {
        return 1; //lint 68 correct: const parameter cannot have written s[0]
    }
    return 0;
}
