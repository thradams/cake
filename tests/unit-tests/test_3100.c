
#define _is_same(T1, T2) _Generic(T1, T2: 1, default: 0)

static_assert(_is_same(typeof(1), int));
static_assert(_is_same(typeof(1L), long));
static_assert(_is_same(typeof(1LL), long long));
static_assert(_is_same(typeof(1U), unsigned int));
static_assert(_is_same(typeof(1ULL), unsigned long long));
static_assert(_is_same(typeof(1), int));
static_assert(_is_same(typeof(1l), long));
static_assert(_is_same(typeof(1ll), long long));
static_assert(_is_same(typeof(1u), unsigned int));
static_assert(_is_same(typeof(1ull), unsigned long long));
static_assert(_is_same(typeof(0x1), int));
static_assert(_is_same(typeof(0x1L), long));
static_assert(_is_same(typeof(0x1LL), long long));
static_assert(_is_same(typeof(0x1U), unsigned int));
static_assert(_is_same(typeof(0x1ULL), unsigned long long));
static_assert(_is_same(typeof(0x1), int));
static_assert(_is_same(typeof(0x1l), long));
static_assert(_is_same(typeof(0x1ll), long long));
static_assert(_is_same(typeof(0x1u), unsigned int));
static_assert(_is_same(typeof(0x1ull), unsigned long long));
static_assert(_is_same(typeof(0b1), int));
static_assert(_is_same(typeof(0b1L), long));
static_assert(_is_same(typeof(0b1LL), long long));
static_assert(_is_same(typeof(0b1U), unsigned int));
static_assert(_is_same(typeof(0b1ULL), unsigned long long));
static_assert(_is_same(typeof(0b1l), long));
static_assert(_is_same(typeof(0b1ll), long long));
static_assert(_is_same(typeof(0b1ul), unsigned long));
static_assert(_is_same(typeof(0b1ull), unsigned long long));
static_assert(_is_same(typeof(1.0f), float));
static_assert(_is_same(typeof(1.0), double));
static_assert(_is_same(typeof(1.0L), long double));
