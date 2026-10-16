#pragma safety enable

int f(int (* _Opt fp)(void))
{
    return fp(); //lint 33
}

int g(int (* _Opt fp)(void))
{
    if (fp)
        return fp();
    return 0;
}

int h(int (*fp)(void))
{
    return fp();
}
