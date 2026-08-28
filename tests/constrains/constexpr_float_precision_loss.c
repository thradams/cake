// Test: constexpr with floating-point initializer not exactly representable in destination type

int main()
{
    constexpr float a = 3.14159265358979; //lint 1950 Error: not exactly representable in type
    constexpr float b = 3.1415;           //lint 1950 Error: not exactly representable in type
    constexpr float ok = 3.25;            // OK: exact in binary
    
    const float a1 = 3.14159265358979; //lint 74  Error: not exactly representable in type
    const float b1 = 3.1415;           //lint 74 Error: not exactly representable in type
    const float ok1 = 3.25;            // OK: exact in binary
    
    return 0;
}
