// https://github.com/thradams/cake/issues/177
enum Color : char {
    red = 0x7e,
    green,
    blue
}; //lint 860 enumerator value outside of underlying type range

int main() {
    return 0;
}
