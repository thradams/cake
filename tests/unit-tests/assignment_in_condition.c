void f(int a, int b)
{
    if (a = b) //lint 91
    {
    }

    while (a = b) //lint 91
    {
    }

    /* extra parentheses silence it */
    if ((a = b))
    {
    }

    if (a == b)
    {
    }
}
