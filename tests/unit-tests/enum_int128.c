/*
 * enums with 128-bit values, fixed underlying type or not.
 * Same results as gcc and clang.
 */
#ifdef __SIZEOF_INT128__
#define I128_MAX ((__int128)(((unsigned __int128)1 << 127) - 1))
#define I128_MIN (-I128_MAX - 1)
#define U128_MAX (~(unsigned __int128)0)

/* 1. fixed underlying type */
enum E1 : __int128 { E1_A };
static_assert(sizeof(enum E1) == 16);
static_assert(_Generic(E1_A, enum E1: 1, default: 0));

int f1(void)
{
    typeof(enum E1) a = E1_A;
    return (int) a;
}

/* 2. the implicit increment passes the 64-bit limit */
enum E2 : __int128 { E2_A = 9223372036854775807LL, E2_B, E2_C };
static_assert(E2_B == (__int128) 9223372036854775807LL + 1);
static_assert(E2_C == (__int128) 9223372036854775807LL + 2);
static_assert(E2_B > 0);

enum E3 : unsigned __int128 { E3_A = 18446744073709551615ULL, E3_B };
static_assert(E3_B == (unsigned __int128) 18446744073709551615ULL + 1);
static_assert((E3_B >> 64) == 1);

/* 3. the limits of the type */
enum E4 : __int128 { E4_MIN = I128_MIN, E4_NEXT, E4_MAX = I128_MAX };
static_assert(E4_MIN < 0);
static_assert(E4_NEXT == I128_MIN + 1);
static_assert(E4_MAX == I128_MAX);

enum E5 : unsigned __int128 { E5_MAX = U128_MAX };
static_assert(E5_MAX == U128_MAX);

/* 4. negative values */
enum E6 : __int128 { E6_A = -1, E6_B };
static_assert(E6_A < 0);
static_assert(E6_B == 0);

/* 5. the initializer is converted to the underlying type */
enum E7 : __int128 { E7_A = (unsigned char) 255, E7_B };
static_assert(E7_B == 256);

/* 6. without fixed type, values that fit in 64 bits keep 64 bits */
enum E10 { E10_A = 18446744073709551615ULL };
static_assert(sizeof(enum E10) == 8);

#endif

int main()
{
    return 0;
}
