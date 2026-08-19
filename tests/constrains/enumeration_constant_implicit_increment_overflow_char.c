// https://github.com/thradams/cake/issues/177
enum Color : char {
    red = 0x7e,
    green,
    blue // Error: implicit increment overflows char (green + 1 == 0x80)
};

int main() {
    return 0;
}
