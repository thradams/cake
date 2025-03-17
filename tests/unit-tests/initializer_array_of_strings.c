

constexpr char a[][4] = {"abc", "de", "f"};
static_assert(a[0][0] == 'a');
static_assert(a[0][1] == 'b');
static_assert(a[0][2] == 'c');
static_assert(a[0][3] == '\0');

static_assert(a[1][0] == 'd');
static_assert(a[1][1] == 'e');
static_assert(a[1][2] == '\0');

static_assert(a[2][0] == 'f');
static_assert(a[2][2] == '\0');

static_assert(_Countof(a) == 3);