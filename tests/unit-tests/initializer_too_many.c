
struct X {
    int a, b, c;
};

//warning: warning: excess elements in initializer 
[[cake::w62]]
constexpr struct X x = {.c = 3, 4};

static_assert(x.a == 0);
static_assert(x.b == 0);
static_assert(x.c == 3);


