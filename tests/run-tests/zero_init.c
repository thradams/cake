/*
   Zero initialization of locals (= {} and = {0}): small objects become direct
   stores of the platform's int16/int32/int64 types, larger ones a call to the
   zero fill helper of the widest store the size and alignment allow, else
   the byte memset. Every byte must be zero, the padding included. The loop
   dirties the objects so that each pass checks a fresh initialization.
*/
#include <stdio.h>
#include <string.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

struct S1 { int a; char* p; double d; };
struct S2 { short a; char b; };
struct Big { int a, b, c, d, e; };
struct Nested { struct S1 s; int x; };
union U { int i; float f; };
struct Bits { int a : 3; int b; };
struct C3 { char c[3]; };

static int all_zero(const void* p, size_t n)
{
    const unsigned char* b = p;
    for (size_t i = 0; i < n; i++)
    {
        if (b[i] != 0)
            return 0;
    }
    return 1;
}

static void test_empty_initializer(void)
{
    for (int k = 0; k < 3; k++)
    {
        struct S1 s1 = {};
        struct S2 s2 = {};
        struct Big big = {};
        struct Nested n = {};
        union U u = {};
        struct Bits bits = {};
        struct C3 c3 = {};
        long long a64[10] = {};
        int a32[3] = {};
        short a16[9] = {};
        char a8[7] = {};

        CHECK(all_zero(&s1, sizeof s1));
        CHECK(all_zero(&s2, sizeof s2));
        CHECK(all_zero(&big, sizeof big));
        CHECK(all_zero(&n, sizeof n));
        CHECK(all_zero(&u, sizeof u));
        CHECK(all_zero(&bits, sizeof bits));
        CHECK(all_zero(&c3, sizeof c3));
        CHECK(all_zero(a64, sizeof a64));
        CHECK(all_zero(a32, sizeof a32));
        CHECK(all_zero(a16, sizeof a16));
        CHECK(all_zero(a8, sizeof a8));

        memset(&s1, 0xFF, sizeof s1);
        memset(&s2, 0xFF, sizeof s2);
        memset(&big, 0xFF, sizeof big);
        memset(&n, 0xFF, sizeof n);
        memset(&u, 0xFF, sizeof u);
        memset(&bits, 0xFF, sizeof bits);
        memset(&c3, 0xFF, sizeof c3);
        memset(a64, 0xFF, sizeof a64);
        memset(a32, 0xFF, sizeof a32);
        memset(a16, 0xFF, sizeof a16);
        memset(a8, 0xFF, sizeof a8);
    }
}

static void test_zero_initializer(void)
{
    for (int k = 0; k < 3; k++)
    {
        struct S1 s1 = { 0 };
        struct Big big = { 0 };
        int a32[3] = { 0 };

        CHECK(all_zero(&s1, sizeof s1));
        CHECK(all_zero(&big, sizeof big));
        CHECK(all_zero(a32, sizeof a32));

        memset(&s1, 0xFF, sizeof s1);
        memset(&big, 0xFF, sizeof big);
        memset(a32, 0xFF, sizeof a32);
    }
}

static void test_compound_literal(void)
{
    for (int k = 0; k < 3; k++)
    {
        struct S1* p1 = &(struct S1){};
        struct Big* pb = &(struct Big){};

        CHECK(all_zero(p1, sizeof *p1));
        CHECK(all_zero(pb, sizeof *pb));

        memset(p1, 0xFF, sizeof *p1);
        memset(pb, 0xFF, sizeof *pb);
    }
}

static void test_values(void)
{
    struct S1 s1 = {};
    CHECK(s1.a == 0);
    CHECK(s1.p == 0);
    CHECK(s1.d == 0.0);

    union U u = {};
    CHECK(u.i == 0);
    CHECK(u.f == 0.0f);

    struct Bits bits = {};
    CHECK(bits.a == 0);
    CHECK(bits.b == 0);
}

int main(void)
{
    test_empty_initializer();
    test_zero_initializer();
    test_compound_literal();
    test_values();
    return failures;
}
