
struct X {
    int a, b, c;
};

constexpr struct X x = { .c = 3, 4 }; //too many initializers
#pragma cake diagnostic check "-E100"

static_assert(x.a == 0);
static_assert(x.b == 0);
static_assert(x.c == 3);

