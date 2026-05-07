#pragma safety enable



bool f()
{
    int* _Opt p = 0;

    //warning: pointer is always null

    return p == 0; //lint 28
}