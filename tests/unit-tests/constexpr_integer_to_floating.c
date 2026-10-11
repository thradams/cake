// integer initializer whose value changes when converted to a floating type
// constexpr: error; const: warning

int main()
{
    constexpr float f1 = 16777217;               //lint 1950
    constexpr float f2 = -16777217;              //lint 1950
    constexpr float f3 = 4294967295U;            //lint 1950
    constexpr double d1 = 9007199254740993LL;    //lint 1950
    constexpr double d2 = 18446744073709551615ULL; //lint 1950
    constexpr float ok1 = 16777216;              // OK
    constexpr double ok2 = 9007199254740992LL;   // OK

    const float g1 = 16777217;                   //lint 74
    const double g2 = 9007199254740993LL;        //lint 74
    const float ok3 = 16777216;                  // OK

    float not_constant = 16777217;               // not const: no diagnostic here
    return 0;
}
