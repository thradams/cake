#pragma safety enable

/*
    An arm of an if does not reach its end when every path inside it jumps
    away, also when its last statement is not a jump (a nested if/else that
    returns on both sides).
*/

int get(void);

void both_arms_return_inside_nested_ifs(int x)
{
    if (x)
    {
        if (get())
            return;
        else
            return;
    }
    else
    {
        if (get())
            return;
        else
            return;
    }
    x = 1; //lint 68
}

void true_arm_returns_inside_nested_if(int x)
{
    int n = 0;
    if (x)
    {
        n = 1;
        if (get())
            return;
        else
            return;
    }
    compile_assert(n == 0);
    compile_assert(x == 0);
}

void else_arm_returns_inside_nested_if(int x)
{
    int n = 0;
    if (x)
    {
        n = 1;
    }
    else
    {
        n = 2;
        if (get())
            return;
        else
            return;
    }
    compile_assert(n == 1);
    compile_assert(x != 0);
}

void both_arms_end_in_an_endless_loop(int x)
{
    if (x)
    {
        for (;;)
        {
        }
    }
    else
    {
        for (;;)
        {
        }
    }
    x = 1; //lint 68
}

void both_arms_end_in_a_switch_that_returns(int x)
{
    if (x)
    {
        switch (get())
        {
        case 1:
            return;
        default:
            return;
        }
    }
    else
    {
        switch (get())
        {
        case 1:
            return;
        default:
            return;
        }
    }
    x = 1; //lint 68
}

void one_arm_ends_in_an_endless_loop(int x)
{
    int n = 0;
    if (x)
    {
        n = 1;
        for (;;)
        {
        }
    }
    compile_assert(n == 0);
    compile_assert(x == 0);
}
