
struct Y {
    int a;
    int ar[3];
    int b;
};

constexpr struct Y y = { .ar[1] = 2, 3, 4 };

static_assert(y.a == 0);
static_assert(y.ar[0] == 0);
static_assert(y.ar[1] == 2);
static_assert(y.ar[2] == 3);
static_assert(y.b == 4);
