/* Cake 0.14.00 x86_msvc */

void test_while_basic(int a)
{
    while (a)
    {
        a--;
    }
}

void test_while_post_decrement(int a)
{
    while (a--)
    {
    }
}

void test_while_may_not_execute(int a)
{
    while (a)
    {
        a--;
    }
}

void test_while_break(int a)
{
    while (a)
    {
        if (a == 5)
        {
            break;
        }
        a--;
    }
}

void test_while_infinite_with_break(int a)
{
    while (a)
    {
        break;
    }
}

void test_do_while_basic(int a)
{
    do
    {
        a--;
    }
    while (a);
}

void test_do_while_zero_entry(void)
{
    int a;

    a = 0;
    do
    {
        a--;
    }
    while (a);
}

void test_do_while_break(int a)
{
    do
    {
        if (a == 1)
        {
            break;
        }
        a--;
    }
    while (a);
}

void test_for_basic(int n)
{
    int i;

    for (i = 0; i < n; i++)
    {
    }
}

void test_for_countdown(int a)
{
    for (; a > 0; a--)
    {
    }
}

void test_for_break(int n)
{
    int i;

    for (i = 0; i < n; i++)
    {
        if (i == 5)
        {
            break;
        }
    }
}

void test_for_no_condition(int a)
{
    for (; ; )
    {
        if (a == 0)
        {
            break;
        }
        a--;
    }
}
