#pragma safety enable

/*
   A while/for loop whose condition is false on entry must still merge
   the zero-iteration exit path. Before the fix, only the post-body
   condition-false map (w_pair2.p_false) was merged back into the
   pre-loop state, so a variable set only inside the loop body
   incorrectly looked "always set" after a loop that may never run.
*/

int unknown();

void while_may_not_run()
{
    int a = 0;
    while (unknown())
    {
        a = 1;
    }
    /* Loop may run zero times: a could still be 0. */
    compile_assert(a == 0 || a == 1);
}

void for_may_not_run()
{
    int a = 0;
    for (int i = 0; unknown(); i++)
    {
        a = 1;
    }
    compile_assert(a == 0 || a == 1);
}
