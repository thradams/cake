#pragma safety enable

int do_while_continue_skips_assignment(int n)
{
    int y;
    do
    {
        if (n) continue;
        y = 1;
    }
    while (0);
    return y; //lint 30
}

int do_while_only_continue()
{
    int x = 0;
    do
    {
        x = 1;
        continue;
    }
    while (x == 0);
    return x; /* ok: reachable */
}
