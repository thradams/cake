#pragma safety enable

/*
   False "unreachable code" when a loop body writes the same variable its
   controlling condition tests.

   flow3 visits a loop body twice (a warm-up pass, then a second pass). On the
   second pass `flag` is already 1 -- written by the first pass's body -- so
   `flag == 0` narrows to always-false and the branch body is reported
   unreachable. But the FIRST iteration can enter it whenever the caller passes
   flag == 0, so the code is reachable and the warning is wrong.

   The reported state must be the union over iterations, not the second pass's
   state alone.

   `continue` is NOT required to trigger this (an earlier guess); the two
   functions below differ only in that, and both warn.
*/

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

/* Control: the body never writes the tested variable, so no narrowing carries
   into the second pass and flow3 correctly stays quiet. */
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
