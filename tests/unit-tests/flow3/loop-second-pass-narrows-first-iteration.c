#pragma safety enable

/* a loop body writing the variable its condition tests must not report the first-iteration branch unreachable: the reported state is the union over iterations */

void warns_without_continue(int n, int flag)
{
    for (int k = 0; k < n; k++)
    {
        if (flag == 0)
        {
            flag = 1; /* warning 68: unreachable code -- but iteration 1 reaches it */
        }
    }
}

void warns_with_continue(int n, int flag)
{
    for (int k = 0; k < n; k++)
    {
        if (flag == 0)
        {
            flag = 1; /* same false report */
            continue;
        }
    }
}

/* control: the body never writes the tested variable */
void ok_body_does_not_write_condition(int n, int flag)
{
    for (int k = 0; k < n; k++)
    {
        if (flag == 0)
        {
            int a = 1;
            (void)a;
        }
    }
}
