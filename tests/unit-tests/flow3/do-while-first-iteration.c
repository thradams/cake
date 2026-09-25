#pragma safety enable

void body_reachable_when_condition_false()
{
    int i = 0;
    do
    {
        i = i + 0; /* ok: first iteration runs without testing the condition */
    }
    while (i != 0);
}

void uninit_condition()
{
    int x;
    do
    {
    }
    while (x); //lint 30
}

int assigned_in_body()
{
    int y;
    do
    {
        y = 1;
    }
    while (0);
    return y; /* ok */
}

int exit_after_iterations(int n)
{
    int k = 0;
    do
    {
        k++;
    }
    while (k < n);
    return 10 / k; /* ok: k >= 1 */
}
