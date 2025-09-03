
#define TYPE_IS(E, T) _Generic(E, T: 1, default: 0)

// less <= than i32 max
static_assert(TYPE_IS(1, int));

// max i32
static_assert(TYPE_IS(2147483647, int));
static_assert(TYPE_IS(-2147483647, int));


#if LONG_MAX > INT_MAX
// max i32 + 1
static_assert(TYPE_IS(2147483648, long));

// maximum i64
static_assert(TYPE_IS(9223372036854775807, long));

// maximum i64 + 1, no choice but use unsigned and warning
static_assert(TYPE_IS(9223372036854775808, unsigned long long));
#pragma cake diagnostic check "-Wimplicitly-unsigned-literal"

#else

#ifdef _WIN32
// max i32 + 1
static_assert(TYPE_IS(2147483648, unsigned long));
#else
// max i32 + 1
static_assert(TYPE_IS(2147483648, long long));
#endif


// maximum i64
static_assert(TYPE_IS(9223372036854775807, long long));

// maximum i64 + 1, no choice but use unsigned and warning
static_assert(TYPE_IS(9223372036854775808, unsigned long long));
#pragma cake diagnostic check "-Wimplicitly-unsigned-literal"
#endif

int main() {}
