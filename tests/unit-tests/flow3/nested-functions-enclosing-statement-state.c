#pragma safety enable

#pragma cake diagnostic warning 85

/*
    The loop, the switch, the condition and the return of the enclosing function do not
    reach a local function or a function literal: they are analyzed as any function.
*/

void call_p0(void (*f)(void));
int call_ret(int (*f)(void));

/* inside a loop of the enclosing function the diagnostics that need iteration_pass == 0 still run */

void d_loop(void)
{
    int a = 1;
    if (a) //lint 85
    {
    }
}

void l_in_loop(void)
{
    for (int i = 0; i < 2; i++)
    {
        static void f(void)
        {
            int a = 1;
            if (a) //lint 85
            {
            }
        }
        f();
    }
}

void x_in_loop(void)
{
    for (int i = 0; i < 2; i++)
    {
        call_p0((static void (void)) {
            int a = 1;
            if (a) //lint 85
            {
            }
        });
    }
}

/* inside a switch of the enclosing function: break leaves the switch of the nested function only */

void l_in_switch(int c)
{
    switch (c)
    {
        case 1:
        {
            static void f(int v)
            {
                switch (v)
                {
                    case 1:
                        break;
                    default:
                        break;
                }
            }
            f(c);
            break;
        }
        default:
            break;
    }
}

void x_in_switch(int c)
{
    switch (c)
    {
        case 1:
        {
            call_p0((static void (void)) {
                int k = 0;
                switch (k)
                {
                    case 0:
                        break;
                    default:
                        break;
                }
            });
            break;
        }
        default:
            break;
    }
}
