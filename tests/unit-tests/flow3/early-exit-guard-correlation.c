#pragma safety enable

int guard_return(int c)
{
    int a[4] = { 0 };
    int* p = 0;
    if (c) { p = a; }
    if (!c) { return 0; }
    return *p; /* c is true here */
}

int guard_return_index(int c)
{
    int a[4] = { 0 };
    int i;
    if (c) { i = 1; } else { i = 4; }
    if (!c) { return 0; }
    return a[i]; /* 1 */
}

int guard_else_return(int c)
{
    int a[4] = { 0 };
    int i;
    if (c) { i = 1; } else { i = 4; }
    if (c) { } else { return 0; }
    return a[i]; /* 1 */
}

int guard_break(int c)
{
    int a[4] = { 0 };
    int i;
    int x = 9;
    if (c) { i = 1; } else { i = 4; }
    do
    {
        x = 0;
        if (!c) { break; }
        return 0;
    }
    while (0);
    return a[i + x]; //lint 70
}

int guard_nested(int c, int d)
{
    int a[4] = { 0 };
    int* p = 0;
    if (c) { p = a; }
    if (d)
    {
        if (!c) { return 0; }
    }
    return *p; //lint 33
}

int guard_wrong_side(int c)
{
    int a[4] = { 0 };
    int* p = 0;
    if (c) { p = a; }
    if (c) { return 0; }
    return *p; //lint 33
}

int break_before_guard(int c, int z)
{
    int a[4] = { 0 };
    int i;
    int x = 9;
    if (c) { i = 1; } else { i = 4; }
    do
    {
        x = 0;
        if (z) { break; }
        if (!c) { return 0; }
        return 1;
    }
    while (0);
    return a[i + x]; //lint 70
}

int goto_before_guard(int c, int z)
{
    int a[4] = { 0 };
    int i;
    int x = 9;
    if (c) { i = 1; } else { i = 4; }
    x = 0;
    if (z) { goto end; }
    if (!c) { return 0; }
    return 1;
end:
    return a[i + x]; //lint 70
}

int guard_after_closed_loop(int c, int n)
{
    int a[4] = { 0 };
    int* p = 0;
    if (c) { p = a; }
    while (n--)
    {
        if (n == 3) { break; }
    }
    if (!c) { return 0; }
    return *p; /* c is true here */
}
