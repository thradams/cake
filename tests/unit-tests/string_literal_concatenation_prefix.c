#define MACRO_JOIN(a, b) a ## #b
#define MAKE_LITERAL(s) MACRO_JOIN(u8, s)
#define DAYS 30

/* unprefixed + prefixed is fine, result takes the prefix (C23 6.4.5) */
const char* s1 = "abc " MAKE_LITERAL(DAYS) " dias";
const char* s2 = "abc " u8"30" " dias";

static_assert(sizeof("a" L"b") == 3 * sizeof(L'a'));
static_assert(sizeof(L"a" "b") == 3 * sizeof(L'a'));
static_assert(sizeof("ab" u"c") == 4 * sizeof(u'a'));
static_assert(sizeof("a" U"b") == 3 * sizeof(U'a'));

/* different prefixes */
const char* s3 = u8"abc" L"def"; //lint 64

/* "abc" L"def" is wchar_t[] */
const char* s4 = "abc" L"def"; //lint 54
