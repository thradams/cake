#pragma nullable enable
#pragma ownership enable


_Bool f()
{
    int  * /*_Opt*/ p =0;
    return p == 0;
}