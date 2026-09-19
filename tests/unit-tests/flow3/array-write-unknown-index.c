#pragma safety enable

/* 85 is off by default and shows the folded condition most directly */
#pragma cake diagnostic warning 85

/* a write through an unpinned index invalidates the whole array; a loop counter is not pinned inside the body (expressions.c octal prefix) */

/* Unknown index: every element of the array becomes unknown. */
int unknown_index(const char* s, int c)
{
    char buffer[16] = { 0 };
    buffer[c] = *s;
    if (buffer[1] == 'o' || buffer[1] == 'O')
    {
        return 1;
    }
    return 0;
}

/* Same write inside a loop. */
int unknown_index_in_loop(const char* s, int c)
{
    char buffer[16] = { 0 };
    while (*s)
    {
        buffer[c] = *s;
        s++;
    }
    if (buffer[1] == 'O')
    {
        return 1;
    }
    return 0;
}

/* Index known per iteration only: the counter the body advances. */
int loop_counter_index(const char* s)
{
    int c = 0;
    char buffer[16] = { 0 };
    while (*s)
    {
        buffer[c] = *s;
        c++;
        s++;
    }
    if (buffer[1] == 'o' || buffer[1] == 'O')
    {
        return 1;
    }
    return 0;
}

/* Same, written as a for loop over the loop variable. */
int for_loop_index(int n)
{
    char buffer[16] = { 0 };
    for (int i = 0; i < n; i++)
    {
        buffer[i] = 'x';
    }
    if (buffer[1] == 'O')
    {
        return 1;
    }
    return 0;
}

/* Compound assignment and increment write the element too. */
int compound_and_increment(int c)
{
    int v[4] = { 0, 0, 0, 0 };
    v[c] += 3;
    if (v[1] == 7)
    {
        return 1;
    }

    int w[4] = { 0, 0, 0, 0 };
    w[c]++;
    if (w[2] == 5)
    {
        return 1;
    }
    return 0;
}
