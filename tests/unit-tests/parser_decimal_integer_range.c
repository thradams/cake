#define IS_TYPE(T, E) _Generic(E, T: 1, default: 0)

#ifdef _WIN32
static_assert(IS_TYPE(int, 0));
static_assert(IS_TYPE(int, /*I32MAX*/ 2147483647));
static_assert(IS_TYPE(unsigned long, /*I32MAX + 1*/ 2147483648));
static_assert(IS_TYPE(unsigned long, /*U32MAX*/ 4294967295));
static_assert(IS_TYPE(long long, /*U32MAX + 1*/ 4294967296));
static_assert(IS_TYPE(long long, /*I64 MAX*/ 9223372036854775807));

//warning: integer literal is too large to be represented in a signed integer type, interpreting as unsigned 
[[cake::w50]]
static_assert(IS_TYPE(unsigned long long, /*I64 MAX + 1*/ 9223372036854775808));

//warning: integer literal is too large to be represented in a signed integer type, interpreting as unsigned 
[[cake::w50]]
static_assert(IS_TYPE(unsigned long long, /*U64 MAX */ 18446744073709551615));


#else

static_assert(IS_TYPE(int, 0));
static_assert(IS_TYPE(int, /*I32MAX*/ 2147483647));
static_assert(IS_TYPE(long, /*I32MAX + 1*/ 2147483648));
static_assert(IS_TYPE(long, /*U32MAX*/ 4294967295));
static_assert(IS_TYPE(long, /*U32MAX + 1*/ 4294967296));
static_assert(IS_TYPE(long, /*I64 MAX*/ 9223372036854775807));
//static_assert(IS_TYPE(__int128, /*I64 MAX + 1*/ 9223372036854775808));
//static_assert(IS_TYPE(__int128, /*U64 MAX */ 18446744073709551615));

#endif
