/*
   __int128 in the generated code, on the targets that have it: sizeof and
   alignof must agree with the real compiler, and the arithmetic must keep
   the high part. Elsewhere __int128 does not exist and there is nothing to run.
*/
#include <stdio.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

#ifdef __SIZEOF_INT128__

struct S
{
    char c;
    __int128 v;
};

static_assert(sizeof(__int128) == 16);

static __int128 twice(__int128 v)
{
    return v * 2;
}

static unsigned __int128 add_one(unsigned __int128 v)
{
    return v + 1;
}

#endif

int main(void)
{
#ifdef __SIZEOF_INT128__
    /* the member offset (computed by the real compiler) agrees with cake's alignof */
    struct S st = { 0 };
    CHECK((size_t)((char*)&st.v - (char*)&st) == alignof(__int128));

    __int128 x = 21;

    /* passed and returned by value */
    CHECK(twice(x) == 42);
    CHECK(add_one(41) == 42);

    __int128 m = -7;
    CHECK(m / 2 == -3);
    CHECK(m % 2 == -1);
    CHECK((m >> 1) == -4);

    unsigned __int128 u = 0;
    u = u - 1;
    CHECK(u > 0);

    /* (2^64 - 1)^2 = 0xfffffffffffffffe0000000000000001 */
    unsigned long long a = 0xFFFFFFFFFFFFFFFFULL;
    unsigned __int128 p = (unsigned __int128)a * a;
    CHECK((unsigned long long)(p >> 64) == 0xFFFFFFFFFFFFFFFEULL);
    CHECK((unsigned long long)p == 1);

    /* the sign is in the high part */
    __int128 s = -1;
    CHECK((long long)(s >> 64) == -1);

    /* division and remainder with a 128-bit dividend */
    unsigned __int128 q = p / 3;
    CHECK(q * 3 + p % 3 == p);

    /* constants wider than 64 bits are folded by cake and written in the generated code */
    unsigned __int128 big = (unsigned __int128)1 << 100;
    CHECK((unsigned long long)(big >> 64) == (1ULL << 36));
    CHECK((unsigned long long)big == 0);

    __int128 minus_big = -(__int128)((unsigned __int128)1 << 100);
    CHECK(minus_big < 0);
    CHECK(-minus_big == (__int128)big);

    unsigned __int128 all_ones = (unsigned __int128)-1;
    CHECK((unsigned long long)(all_ones >> 64) == 0xFFFFFFFFFFFFFFFFULL);
#endif

    return failures;
}
