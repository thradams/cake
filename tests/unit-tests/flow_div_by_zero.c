
#pragma safety enable

void f(int i)
{
    if (i < 0)
    {
        int k = 1 / i;
    }
    else
    {
        //possible division by zero
        [[cake::w36]]
        int j = 1 / i;
    }

    if (0 > i)
    {
        int k = 1 / i;
    }
    else
    {
        //possible division by zero
        [[cake::w36]]
        int j = 1 / i;
    }

    if (0 < i)
    {
        int k = 1 / i;
    }
    else
    {
        //possible division by zero
        [[cake::w36]]
        int j = 1 / i;
    }

    if (i > 0)
    {
        int k = 1 / i;
    }
    else
    {
        //possible division by zero
        [[cake::w36]]
        int j = 1 / i;
    }
}


void f2(int i)
{
    if (i <= 0)
    {
        //possible division by zero
        [[cake::w36]]
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i;

    }

    if (0 >= i)
    {
        //possible division by zero
        [[cake::w36]]
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i;
    }

    if (0 <= i)
    {
        //possible division by zero
        [[cake::w36]]
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i;

    }


    if (i >= 0)
    {
        //possible division by zero
        [[cake::w36]]
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i;

    }
}

