static_assert(92 == 0134);

static_assert('\0' == 0);
static_assert('\n' == 10);
static_assert('\r' == 13);
static_assert('\'' == 0x27);
static_assert('\\' == 0x5C);
static_assert('\x5C' == 0x5C);
static_assert('\x5c' == 0x5C);

static_assert('\0' == 0);
static_assert('\01' == 01);
static_assert('\02' == 02);

static_assert(u8'\n' == 10);
static_assert(u8'\r' == 13);
static_assert(u8'\'' == 0x27);
static_assert(u8'\\' == 0x5C);
static_assert(u8'\x5C' == 0x5C);
static_assert(u8'\x5c' == 0x5C);


/*TODO MORE TESTS*/
