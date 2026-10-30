#pragma safety enable

/* *p as a condition narrows the values of its targets */

void one_target(void)
{
    int a = 1;
    int* p = &a;
    *p = 0;
    if (*p)
    {
        a = 99; //lint 68 unreachable code
    }
}

void two_targets(int c)
{
    int a = 1;
    int b = 2;
    int* p = &a;
    if (c) { p = &b; }
    *p = 0;
    if (*p)
    {
        a = 99; //lint 68 unreachable code
    }
}

void two_targets_one_true(int c)
{
    int a = 0;
    int b = 0;
    int* p = &a;
    if (c) { p = &b; *p = 1; }
    if (*p)
    {
        compile_assert(b == 1);
    }
    else
    {
        compile_assert(a == 0);
    }
}

void not_content(int c)
{
    int a = 1;
    int b = 2;
    int* p = &a;
    if (c) { p = &b; }
    *p = 0;
    compile_assert(!*p);
}
