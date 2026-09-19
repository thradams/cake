#pragma safety enable

/* a loop whose condition is false on entry merges the zero-iteration exit: a variable set only in the body is not "always set" */

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
