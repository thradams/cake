//https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3030.htm
enum X : short {
    A
};

int main() {
    enum X x = A;
    static_assert(sizeof(x) == sizeof(short));
}
