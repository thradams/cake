constexpr struct { int a[3], b; } w[] =
   { [0].a = {1}, [1].a[0] = 2 };

static_assert(w[0].a[0] == 1);
static_assert(w[1].a[0] == 2);
