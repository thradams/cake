// https://github.com/thradams/cake/issues/240

int main() {
    constexpr unsigned int a = 0;
    constexpr unsigned int b = 1;
    constexpr unsigned int c = -1;              //lint 1950 error: constant expression is not exactly representable in type
    constexpr unsigned int d = -2;               //lint 1950 error: constant expression is not exactly representable in type

    constexpr int e = -1;
    constexpr int f = 2147483647;
    constexpr int g = 2147483648;                //lint 1950 error: constant expression is not exactly representable in type

    constexpr unsigned char h = 255;
    constexpr unsigned char i = 256;              //lint 1950 error: constant expression is not exactly representable in type

    const unsigned int j = -1;                    //lint 74 warning: constant expression is not exactly representable in type
    unsigned int k = -1;                          //lint 74 warning: constant expression is not exactly representable in type
}
