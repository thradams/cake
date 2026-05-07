void T00()
{
    //warning: warning: excess elements in initializer

    constexpr int i[2] = {1, 2, 3}; //lint 62

    //error: array index '3' in initializer exceeds array bounds

    constexpr int i2[2] = {1,[3] = 2}; //lint 720

    //error: array designator value '-1' is negative

    constexpr int i3[2] = {1,[2 - 3] = 2}; //lint 720

    constexpr int i5 = {{1}}; //ok

    //warning: warning: excess elements in initializer

    constexpr int i4 = {1, 2}; //lint 62

}