
#pragma safety enable

void f(int i)
{
    if (i < 0)
    {
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i; //lint 36 possible division by zero
    }

    if (0 > i)
    {
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i; //lint 36 possible division by zero
    }

    if (0 < i)
    {
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i; //lint 36 possible division by zero
    }

    if (i > 0)
    {
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i; //lint 36 possible division by zero
    }
}


void f2(int i)
{
    if (i <= 0)
    {
        int k = 1 / i; //lint 36 possible division by zero
    }
    else
    {
        int j = 1 / i;

    }

    if (0 >= i)
    {
        int k = 1 / i; //lint 36 possible division by zero
    }
    else
    {
        int j = 1 / i;
    }

    if (0 <= i)
    {
        int k = 1 / i; //lint 36 possible division by zero
    }
    else
    {
        int j = 1 / i;

    }


    if (i >= 0)
    {
        int k = 1 / i; //lint 36 possible division by zero
    }
    else
    {
        int j = 1 / i;

    }
}

