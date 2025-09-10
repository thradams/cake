void T00()
{
    //warning: warning: excess elements in initializer
    [[cake::w100]]
    constexpr int i[2] = {1, 2, 3};

    //error: array index '3' in initializer exceeds array bounds
    [[cake::e720]]
    constexpr int i2[2] = {1,[3] = 2};

    //error: array designator value '-1' is negative
    [[cake::e720]]
    constexpr int i3[2] = {1,[2 - 3] = 2};

    constexpr int i5 = {{1}}; //ok

    //warning: warning: excess elements in initializer
    [[cake::w100]]
    constexpr int i4 = {1, 2};

}