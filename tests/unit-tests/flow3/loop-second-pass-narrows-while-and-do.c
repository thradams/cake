#pragma safety enable

/* FIXED for while/do: flow_join_first_iteration_values unions pre-loop and after-one-iteration values per key, skipping keys with ownership/lifetime facts (free-and-advance stays sound) */

void with_for(int n, int flag)
{
    for (int k = 0; k < n; k++)
    {
        if (flag == 0)
        {
            flag = 1;
        }
    }
}

void with_while(int n, int flag)
{
    int k = 0;
    while (k < n)
    {
        if (flag == 0)
        {
            flag = 1; /* reachable on the first iteration when flag == 0 */
        }
        k++;
    }
}

void with_do_while(int n, int flag)
{
    int k = 0;
    do
    {
        if (flag == 0)
        {
            flag = 1; /* reachable on the first iteration when flag == 0 */
        }
        k++;
    } while (k < n);
}
