// floating initializer whose value changes when converted to an integer type
// constexpr: error; const: warning

int main()
{
    constexpr int i = 1.5;            //lint 1950
    constexpr unsigned u = -1.0;      //lint 1950
    constexpr bool b = 0.5;           //lint 1950
    constexpr signed char c = 300.0;  //lint 1950
    constexpr int ok1 = 2.0;          // OK
    constexpr unsigned char ok2 = 255.0; // OK

    const int i1 = 1.5;               //lint 74
    const unsigned char c1 = 256.0;   //lint 74
    const int ok3 = 2.0;              // OK

    int not_constant = 1.5;           // not const: no diagnostic here
    return 0;
}
