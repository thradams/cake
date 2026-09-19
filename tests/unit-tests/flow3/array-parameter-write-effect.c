#pragma safety enable

/* passing an array to a non-const array parameter invalidates its contents, same as a pointer parameter (expressions.c suffix[4]) */

/* A: callee only declared */
void fill_decl(char s[4]);

int declared_callee(void)
{
    char s[4] = { 0 };
    fill_decl(s);
    if (s[0] == 'F')
    {
        return 1; /* ok: reachable, and correctly not reported */
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
        return 1; /* ok: reachable, and correctly not reported */
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

/* D: a const array parameter cannot write, so the fold is right and the body really is unreachable */
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
