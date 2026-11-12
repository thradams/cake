#pragma safety enable

#pragma cake diagnostic warning 85

int f(int c)
{
    int x = 1;
    if (c)
    {
        x = 2;
    }
    int r = 0;
    /* x is 1 or 2: one report for both values */
    if (x != 0) //lint 85
    {
        r = 1;
    }
    return r;
}

int g(int c)
{
    int a = 1;
    int b = 2;
    int* p = &a;
    if (c)
    {
        p = &b;
    }
    int r = 0;
    /* *p is a or b: one report for both targets */
    if (*p != 0) //lint 85
    {
        r = 1;
    }
    return r;
}

int h(int c)
{
    int x = 0;
    if (c)
    {
        x = 2;
    }
    int r = 0;
    /* x is 0 or 2: no report */
    if (x != 0)
    {
        r = 1;
    }
    return r;
}
