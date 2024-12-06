void T00()
{
    constexpr int i[2] = { 1, 2, 3 };
#pragma cake diagnostic check "-E100"

    constexpr int i2[2] = { 1,[3] = 2 };
#pragma cake diagnostic check "-E720"

    constexpr int i3[2] = { 1,[2 - 3] = 2 };
#pragma cake diagnostic check "-E720"

    constexpr int i5 = { {1} }; //ok

    constexpr int i4 = { 1, 2 };
#pragma cake diagnostic check "-E100"

}