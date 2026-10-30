#pragma safety enable



bool f()
{
    int* p = 0;

    //warning: pointer is always null

    return p == 0;
}
