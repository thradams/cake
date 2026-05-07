
struct X {
    int a, b, c;
};

//warning: warning: excess elements in initializer 

constexpr struct X x = {.c = 3, 4};//lint 62

static_assert(x.a == 0);
static_assert(x.b == 0);
static_assert(x.c == 3);


