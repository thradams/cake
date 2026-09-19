#pragma safety enable

/* several break sites must accumulate into the break-join map, not overwrite each other */

int unknown();

void multi_break_while(int x)
{
    int a = 0;
    while (unknown())
    {
        if (x == 1)
        {
            a = 1;
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

    /* all three break sites plus the zero-iteration exit must be visible after the loop */
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
