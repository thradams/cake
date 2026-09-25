#pragma safety enable

int increment_signed(int k)
{
    if (k >= 0)
    {
        k++;
        return 10 / k; /* ok: k >= 1 */
    }
    return 0;
}

int while_may_not_run(int n)
{
    int k = 0;
    while (k < n)
    {
        k++;
    }
    return 10 / k; //lint 36
}

unsigned increment_unsigned(unsigned k)
{
    if (k > 0)
    {
        k++;
        return 10 / k; //lint 36
    }
    return 0;
}

int plus_assign_keeps_relation(int n)
{
    int k = 0;
    do
    {
        k += 2;
    }
    while (k < n);
    return 10 / k; /* ok: k >= 2 */
}

int minus_assign_keeps_relation(int n)
{
    int k = 0;
    do
    {
        k -= 3;
    }
    while (k > n);
    return 10 / k; /* ok: k <= -3 */
}

int non_step_widens_to_any(int n)
{
    int k = 0;
    do
    {
        k = 3 - 2 * k;
    }
    while (k < n);
    return 10 / k; //lint 36
}

int a8[8];

int minus_keeps_relation_value(int n)
{
    if (n >= 1 && n <= 8)
    {
        int i = n - 1;
        return a8[i]; /* ok: i >= 0, not a sentinel value */
    }
    return 0;
}
