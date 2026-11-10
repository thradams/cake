#pragma safety enable

int f(int (* _Nullable fp)(void))
{
    return fp(); //lint 33
}

int g(int (* _Nullable fp)(void))
{
    if (fp)
        return fp();
    return 0;
}

int h(int (*fp)(void))
{
    return fp();
}
