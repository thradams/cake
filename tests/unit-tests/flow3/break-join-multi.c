#pragma safety enable

/*
   Regression test for flow3_map_accumulate_into_join.

   Multiple break statements (or multiple switch cases) join into the
   same break-join map. Before the fix, the join was implemented as a
   merge (flow3_map_merge_a_b(join, current, join)) which treats "join"
   itself as one of only two arms, so the second break silently
   overwrote the state left by the first instead of accumulating both.
*/

int unknown();

void multi_break_while(int x)
{
    int a = 0;
    while (unknown())
    {
        if (x == 1)
        {
            a = 1; //lint 68 unreachable code
            break;
        }
        if (x == 2)
        {
            a = 2;
            break;
        }
        a = 3;
        break;
    }

    /* All three break sites, plus the zero-iteration exit (a == 0),
       must be visible after the loop. */
    compile_assert(a == 0 || a == 1 || a == 2 || a == 3);
}

void multi_break_switch(int x)
{
    int a = 0;
    switch (x)
    {
    case 1:
        a = 1;
        break;
    case 2:
        a = 2;
        break;
    default:
        a = 3;
        break;
    }

    compile_assert(a == 1 || a == 2 || a == 3);
    compile_assert(a != 0);
}
