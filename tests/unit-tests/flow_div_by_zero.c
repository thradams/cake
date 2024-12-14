
//TODO a > 1 etc..

#pragma safety enable


void f(int i)
{
    if (i < 0)
    {
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i;
#pragma cake diagnostic check "-Wflow-div-by-zero"
    }

    if (0 > i)
    {
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i;
#pragma cake diagnostic check "-Wflow-div-by-zero"
    }

    if (0 < i)
    {
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i;
#pragma cake diagnostic check "-Wflow-div-by-zero"
    }


    if (i > 0)
    {
        int k = 1 / i;
    }
    else
    {
        int j = 1 / i;
#pragma cake diagnostic check "-Wflow-div-by-zero"
    }
}


void f2(int i)
{
    if (i <= 0)
    {
        int k = 1 / i;
#pragma cake diagnostic check "-Wflow-div-by-zero"
    }
    else
    {
        int j = 1 / i;

    }

    if (0 >= i)
    {
        int k = 1 / i;
#pragma cake diagnostic check "-Wflow-div-by-zero"
    }
    else
    {
        int j = 1 / i;
    }

    if (0 <= i)
    {
        int k = 1 / i;
#pragma cake diagnostic check "-Wflow-div-by-zero"
    }
    else
    {
        int j = 1 / i;

    }


    if (i >= 0)
    {
        int k = 1 / i;
#pragma cake diagnostic check "-Wflow-div-by-zero"
    }
    else
    {
        int j = 1 / i;

    }
}

