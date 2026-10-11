// a const/constexpr object initialized with a compound literal of constants
// is evaluated as if initialized with the braced initializer itself

struct P { int x, y; };

constexpr struct P p1 = (struct P){ 1, 2 };
static_assert(p1.y == 2);

const struct P p2 = (struct P){ .y = 7 };
static_assert(p2.x == 0 && p2.y == 7);

void f(void)
{
    constexpr struct X { int x; } x = (struct X){ .x = 50 };
    static_assert(x.x == 50);

    const int i = (int){ 5 };
    static_assert(i == 5);

    constexpr unsigned char c1 = (int){ 300 }; //lint 1950
    const unsigned char c2 = (int){ 300 };     //lint 74

    struct P not_const = (struct P){ 1, 2 };
    static_assert(not_const.y == 2); //lint 960
}

int main() {
    return 0;
}
