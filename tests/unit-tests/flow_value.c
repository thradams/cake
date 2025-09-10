#pragma safety enable



bool f()
{
    int* _Opt p = 0;

    //warning: pointer is always null
    [[cake::w28]]
    return p == 0;
}