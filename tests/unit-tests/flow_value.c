#pragma safety enable



bool f()
{
    int  * _Opt p =0;
    return p == 0;
    #pragma cake diagnostic check "-Wflow-not-null"
}