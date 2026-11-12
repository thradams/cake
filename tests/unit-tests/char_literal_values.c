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


static_assert('\?' == '?');
static_assert('\?' == 0x3F);
static_assert(u8'\?' == 0x3F);
static_assert(U'\?' == 0x3F);
static_assert(L'\?' == 0x3F);

static_assert(U'é' == 0xE9);
static_assert(U'😀' == 0x1F600);
static_assert(U'$' == '$');

/* an octal or hexadecimal escape is the value of the code unit */
static_assert(u8'\xff' == 255);
static_assert(u8'\x80' == 128);
static_assert(u8'\377' == 255);
static_assert(u8'\x7f' == 127);

/*TODO MORE TESTS*/
