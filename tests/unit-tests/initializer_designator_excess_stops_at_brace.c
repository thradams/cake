
struct X
{
   int a;
   int b;
   int c;
};

struct Y
{
   struct X x;
   int d;
};

//warning: warning: excess elements in initializer

constexpr struct Y y = { .x = {.a =1, 2, 3, 4 /*not used*/}};//lint 62

static_assert(y.x.a == 1);
static_assert(y.x.b == 2);
static_assert(y.x.c == 3);
static_assert(y.d == 0);

