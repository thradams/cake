enum Color2 : char {
    red2 = 0x7d,
    green2,
    blue2
};

int main() {
    static_assert(red2 == 0x7d);
    static_assert(green2 == 0x7e);
    static_assert(blue2 == 0x7f);
}
