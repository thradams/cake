/*
   Bitfields: reads, stores, compound assignments, ++/--, initializers and
   that a store does not change the neighbor members. The generated-code
   tests also run with -no-bitfields, where the bitfields are stored in plain
   unsigned members.
*/
#include <stdio.h>
#include <string.h>

static int failures = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); failures++; } } while(0)

struct Flags { unsigned enabled : 1; unsigned mode : 3; unsigned error : 1; };
struct S1 { unsigned a : 3; int b : 5; char c; unsigned d : 20; };
struct S2 { char a; int b : 20; char c; int e : 4; };
struct S3 { unsigned char a : 3; unsigned int b : 9; unsigned short f : 10; };
struct S4 { char a : 7; char b : 3; int : 0; int c : 2; long long g : 40; int h : 30; };
struct S5 { int a : 4; char x; short s : 9; short t : 9; };
union U1 { int a : 3; char b : 5; };
struct S6 { int k; struct { int m : 5; unsigned n : 7; }; unsigned last : 31; };
struct S7 { unsigned u : 32; int i : 32; unsigned long long w : 64; long long v : 33; };

struct Flags g_flags = { 1, 5, 1 };
struct Flags g_designated = { .mode = 6 };
struct S4 g_s4 = { -50, -2, 1, -123456789012LL, -400000000 };
struct S6 g_s6 = { 11, { -16, 100 }, 0x7FFFFFFE };

static int five(void) { return 5; }

static void test_globals(void)
{
    CHECK(g_flags.enabled == 1 && g_flags.mode == 5 && g_flags.error == 1);
    CHECK(g_designated.enabled == 0 && g_designated.mode == 6 && g_designated.error == 0);
    CHECK(g_s4.a == -50 && g_s4.b == -2 && g_s4.c == 1);
    CHECK(g_s4.g == -123456789012LL && g_s4.h == -400000000);
    CHECK(g_s6.k == 11 && g_s6.m == -16 && g_s6.n == 100 && g_s6.last == 0x7FFFFFFE);
}

static void test_store_keeps_neighbors(void)
{
    struct S1 s;
    memset(&s, 0xA5, sizeof s);
    s.a = 5;
    s.b = -9;
    s.d = 0xABCDE;
    CHECK(s.a == 5 && s.b == -9 && s.c == (char)0xA5 && s.d == 0xABCDE);

    s.a = five() + 8; /* 13 wraps to 3 bits */
    CHECK(s.a == 5 && s.b == -9);

    struct S2 s2;
    memset(&s2, 0xA5, sizeof s2);
    s2.b = -300000;
    s2.e = five() + 4; /* 9 in 4 bits: -7 */
    CHECK(s2.a == (char)0xA5 && s2.b == -300000 && s2.c == (char)0xA5 && s2.e == -7);

    struct S3 s3 = { 0 };
    s3.a = 6;
    s3.b = 400;
    s3.f = 1000;
    CHECK(s3.a == 6 && s3.b == 400 && s3.f == 1000);

    struct S5 s5;
    memset(&s5, 0xA5, sizeof s5);
    s5.a = -8;
    s5.s = -200;
    s5.t = 255;
    CHECK(s5.a == -8 && s5.x == (char)0xA5 && s5.s == -200 && s5.t == 255);

    union U1 u;
    u.a = -3;
    CHECK(u.a == -3);
}

static void test_wide_fields(void)
{
    struct S7 s = { 0 };
    s.u = 0xFFFFFFF0u;
    s.i = -2000000000;
    s.w = 0xFEDCBA9876543210ULL;
    s.v = -4000000000LL;
    CHECK(s.u == 0xFFFFFFF0u && s.i == -2000000000);
    CHECK(s.w == 0xFEDCBA9876543210ULL && s.v == -4000000000LL);
    CHECK(s.u + 1 == 0xFFFFFFF1u); /* unsigned :32 stays unsigned */

    struct S4 s4 = { 0 };
    s4.g = -123456789012LL;
    s4.h = -400000000;
    CHECK(s4.g == -123456789012LL && s4.h == -400000000 && s4.c == 0);
}

static void test_operators(void)
{
    struct S1 s = { 0 };
    int value = 0;

    CHECK((s.a = 5) == 5);
    CHECK((s.a = five() + 8) == 5); /* the value of the assignment is the stored value */

    s.a += 3;
    CHECK(s.a == 0);
    s.b = 7;
    s.b -= 10;
    CHECK(s.b == -3);
    s.b *= 5;
    CHECK(s.b == -15);
    s.d = 0xFFF00;
    s.d |= 0x10;
    s.d &= 0xFFF1F;
    s.d ^= 0x3;
    CHECK(s.d == 0xFFF13);
    s.d <<= 4;
    CHECK(s.d == 0xFF130);
    s.d >>= 8;
    CHECK(s.d == 0xFF1);

    s.a = 6;
    value = s.a++;
    CHECK(value == 6 && s.a == 7);
    value = s.a++; /* wraps */
    CHECK(value == 7 && s.a == 0);
    value = ++s.a;
    CHECK(value == 1 && s.a == 1);
    value = s.a--;
    CHECK(value == 1 && s.a == 0);
    value = --s.a;
    CHECK(value == 7 && s.a == 7);

    s.b = 15;
    s.b++; /* 5 bits signed: -16 */
    CHECK(s.b == -16);

    struct S2 s2 = { 0 };
    s2.b = 2.75;
    CHECK(s2.b == 2);
    s2.b = -1.5;
    CHECK(s2.b == -1);

    struct S5 a[2] = { 0 };
    int i = 0;
    a[i].a = 3;
    i++;
    a[i].a = -2;
    CHECK(a[0].a == 3 && a[1].a == -2);

    struct Flags f = { 0 };
    for (int k = 0; k < 10; k++)
        f.mode++;
    CHECK(f.mode == 2 && f.enabled == 0 && f.error == 0);
}

static void test_local_initializers(void)
{
    struct Flags f0 = { 0 };
    CHECK(f0.enabled == 0 && f0.mode == 0 && f0.error == 0);

    struct Flags fd = { .error = 1, .mode = 3 };
    CHECK(fd.enabled == 0 && fd.mode == 3 && fd.error == 1);

    int m = five();
    struct Flags fn = { 1, m, 1 };
    CHECK(fn.enabled == 1 && fn.mode == 5 && fn.error == 1);

    struct S2 s2 = { 7, m * -1000, 8, m - 9 };
    CHECK(s2.a == 7 && s2.b == -5000 && s2.c == 8 && s2.e == -4);

    struct S4 s4 = { -50, -2, 1, five() * -100000000000LL, -five() };
    CHECK(s4.a == -50 && s4.b == -2 && s4.c == 1);
    CHECK(s4.g == -500000000000LL && s4.h == -5);

    struct S6 s6 = { five(), { -five(), 100 }, 7 };
    CHECK(s6.k == 5 && s6.m == -5 && s6.n == 100 && s6.last == 7);

    struct S4 copy = g_s4;
    CHECK(copy.a == -50 && copy.c == 1 && copy.g == -123456789012LL);

    union U1 u = { five() - 8 };
    CHECK(u.a == -3);
}

int main(void)
{
    test_globals();
    test_store_keeps_neighbors();
    test_wide_fields();
    test_operators();
    test_local_initializers();
    return failures;
}
