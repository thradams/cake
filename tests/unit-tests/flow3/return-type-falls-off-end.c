#pragma flow enable

void abort(void);

int f1(int a)
{
    if (a > 0)
        return 1;
} //lint 98 control reaches end of non-void function

int f2(int a)
{
    if (a > 0)
        return 1;
    else
        return 2;
}

int f3(int a)
{
    for (;;)
    {
        if (a)
            return 1;
    }
}

int f4(int a)
{
    switch (a)
    {
    case 1: return 1;
    default: return 2;
    }
}

int f5(int a)
{
    try
    {
        if (a) throw;
        return 1;
    }
    catch
    {
        return 2;
    }
}

int f6(int a)
{
    while (1)
    {
        if (a)
            return 1;
    }
}

int f7(int a)
{
    switch (a)
    {
    case 1: return 1;
    }
} //lint 98 control reaches end of non-void function

int main(void)
{
}
