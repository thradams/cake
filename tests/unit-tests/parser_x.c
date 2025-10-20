
static_assert("\x1bHello"[0] == 0x1b);
static_assert("\x1bHello"[0] == '\x1b');
static_assert(sizeof(u8"\x1bHello\n") == 8);
static_assert(sizeof("\x1bHello\n") == 8);
static_assert(sizeof(L"\x1bHello\n") == sizeof(L' ') * 8);
static_assert(sizeof(L"maçã") == sizeof(L' ') * (4 + 1));

static_assert(sizeof("maçã") == 7);
static_assert(sizeof(u8"maçã") == 7);

