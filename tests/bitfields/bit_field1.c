/*
 * test_bitfields.c
 * Unit tests for C bitfield behavior.
 * Compile: gcc -std=c11 -Wall -Wextra -o test_bitfields test_bitfields.c
 * Run:     ./test_bitfields
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

/* ─── Minimal test framework ─────────────────────────────────────── */
static int tests_run    = 0;
static int tests_passed = 0;

#define ASSERT(desc, expr)                                          \
    do {                                                            \
        tests_run++;                                                \
        if (expr) {                                                 \
            tests_passed++;                                         \
            printf("  [PASS] %s\n", desc);                         \
        } else {                                                    \
            printf("  [FAIL] %s  (line %d)\n", desc, __LINE__);    \
        }                                                           \
    } while (0)

#define SECTION(name) printf("\n=== %s ===\n", name)

/* ─── Bitfield structs used across tests ─────────────────────────── */

/* 1-bit to 4-bit unsigned fields */
typedef struct {
    unsigned int a : 1;
    unsigned int b : 2;
    unsigned int c : 3;
    unsigned int d : 4;
} UBits;

/* Signed fields */
typedef struct {
    int s1 : 1;
    int s3 : 3;
    int s7 : 7;
} SBits;

/* Mixed signed / unsigned */
typedef struct {
    unsigned int flag   : 1;
    int          offset : 7;
    unsigned int type   : 4;
} MixedBits;

/* Unnamed padding field */
typedef struct {
    unsigned int x      : 4;
    unsigned int        : 4;   /* 4-bit padding, no name */
    unsigned int y      : 4;
} PaddedBits;

/* Status register (a realistic example) */
typedef struct {
    unsigned int carry     : 1;
    unsigned int zero      : 1;
    unsigned int overflow  : 1;
    unsigned int negative  : 1;
    unsigned int reserved  : 4;
} StatusReg;

/* Network header (a realistic example) */
typedef struct {
    unsigned int version  : 4;
    unsigned int ihl      : 4;
    unsigned int dscp     : 6;
    unsigned int ecn      : 2;
} IPv4Header;

/* ─── Tests ──────────────────────────────────────────────────────── */

/* 1. Basic read / write */
static void test_basic_read_write(void)
{
    SECTION("Basic read / write");
    UBits u = {0};

    u.a = 1;
    ASSERT("1-bit field stores 1",       u.a == 1);

    u.b = 3;
    ASSERT("2-bit field stores 3",       u.b == 3);

    u.c = 7;
    ASSERT("3-bit field stores 7",       u.c == 7);

    u.d = 15;
    ASSERT("4-bit field stores 15",      u.d == 15);

    /* Verify fields are independent */
    ASSERT("a unaffected after d set",   u.a == 1);
    ASSERT("b unaffected after d set",   u.b == 3);
    ASSERT("c unaffected after d set",   u.c == 7);
}

/* 2. Overflow / wrap-around (unsigned) */
static void test_overflow_unsigned(void)
{
    SECTION("Unsigned overflow / wrap-around");
    UBits u = {0};

    u.a = 2;         /* 2 into 1-bit → wraps to 0 */
    ASSERT("1-bit: 2 wraps to 0",        u.a == 0);

    u.b = 4;         /* 4 into 2-bit → wraps to 0 */
    ASSERT("2-bit: 4 wraps to 0",        u.b == 0);

    u.b = 5;         /* 5 (0b101) into 2-bit → 1 */
    ASSERT("2-bit: 5 wraps to 1",        u.b == 1);

    u.d = 16;        /* 16 into 4-bit → wraps to 0 */
    ASSERT("4-bit: 16 wraps to 0",       u.d == 0);

    u.d = 17;        /* 17 (0b10001) → 1 */
    ASSERT("4-bit: 17 wraps to 1",       u.d == 1);
}

/* 3. Signed fields — two's complement range */
static void test_signed_fields(void)
{
    SECTION("Signed fields");
    SBits s = {0};

    /* 1-bit signed: only representable value with sign bit set is -1 */
    s.s1 = -1;
    ASSERT("s1 stores -1",               s.s1 == -1);

    s.s1 = 1;        /* overflows — 1 in a 1-bit signed field = -1 */
    ASSERT("s1: 1 wraps to -1",          s.s1 == -1);

    /* 3-bit signed: range [-4, 3] */
    s.s3 = 3;
    ASSERT("s3 stores 3",                s.s3 == 3);

    s.s3 = -4;
    ASSERT("s3 stores -4",               s.s3 == -4);

    s.s3 = 4;        /* overflows: 4 (0b100) sign-extended = -4 */
    ASSERT("s3: 4 wraps to -4",          s.s3 == -4);

    /* 7-bit signed: range [-64, 63] */
    s.s7 = 63;
    ASSERT("s7 stores 63",               s.s7 == 63);

    s.s7 = -64;
    ASSERT("s7 stores -64",              s.s7 == -64);
}

/* 4. Mixed signed / unsigned struct */
static void test_mixed_struct(void)
{
    SECTION("Mixed signed / unsigned fields");
    MixedBits m = {0};

    m.flag   = 1;
    m.offset = -5;
    m.type   = 9;

    ASSERT("flag  = 1",                  m.flag   == 1);
    ASSERT("offset = -5",                m.offset == -5);
    ASSERT("type  = 9",                  m.type   == 9);

    /* Verify independence */
    m.type = 0xF;
    ASSERT("flag unaffected (type=0xF)", m.flag   == 1);
    ASSERT("offset unaffected",          m.offset == -5);
}

/* 5. Zero-width (unnamed) padding */
static void test_padding_field(void)
{
    SECTION("Unnamed padding / zero-width field");
    PaddedBits p = {0};

    p.x = 5;
    p.y = 10;

    ASSERT("x = 5",                      p.x == 5);
    ASSERT("y = 10 (across 4-bit gap)",  p.y == 10);
    ASSERT("x unchanged after y set",    p.x == 5);
}

/* 6. Realistic: status register */
static void test_status_register(void)
{
    SECTION("Realistic: CPU status register");
    StatusReg sr = {0};

    sr.carry    = 1;
    sr.zero     = 1;
    sr.overflow = 0;
    sr.negative = 1;

    ASSERT("carry = 1",                  sr.carry    == 1);
    ASSERT("zero = 1",                   sr.zero     == 1);
    ASSERT("overflow = 0",               sr.overflow == 0);
    ASSERT("negative = 1",               sr.negative == 1);
    ASSERT("reserved = 0",               sr.reserved == 0);

    /* Clear carry, verify others */
    sr.carry = 0;
    ASSERT("after clear: carry = 0",     sr.carry    == 0);
    ASSERT("after clear: zero unchanged",sr.zero     == 1);
    ASSERT("after clear: neg unchanged", sr.negative == 1);
}

/* 7. Realistic: IPv4 header nibbles */
static void test_ipv4_header(void)
{
    SECTION("Realistic: IPv4 header fields");
    IPv4Header hdr = {0};

    hdr.version = 4;
    hdr.ihl     = 5;
    hdr.dscp    = 46;   /* Expedited Forwarding */
    hdr.ecn     = 0;

    ASSERT("version = 4",                hdr.version == 4);
    ASSERT("ihl = 5",                    hdr.ihl     == 5);
    ASSERT("dscp = 46 (EF)",             hdr.dscp    == 46);
    ASSERT("ecn = 0",                    hdr.ecn     == 0);

    hdr.ecn = 3;    /* CE — Congestion Encountered */
    ASSERT("ecn = 3 (CE)",               hdr.ecn     == 3);
    ASSERT("dscp unaffected",            hdr.dscp    == 46);
}

/* 8. memset to all-ones, then read fields */
static void test_all_ones(void)
{
    SECTION("All-ones initialisation");
    UBits u;
    memset(&u, 0xFF, sizeof u);

    ASSERT("1-bit unsigned reads 1 after 0xFF", u.a == 1);
    ASSERT("2-bit unsigned reads 3 after 0xFF", u.b == 3);
    ASSERT("3-bit unsigned reads 7 after 0xFF", u.c == 7);
    ASSERT("4-bit unsigned reads 15 after 0xFF",u.d == 15);
}

/* 9. Zero-initialisation */
static void test_zero_init(void)
{
    SECTION("Zero initialisation");
    UBits  u = {0};
    SBits  s = {0};
    StatusReg sr = {0};

    ASSERT("UBits a = 0",   u.a == 0);
    ASSERT("UBits b = 0",   u.b == 0);
    ASSERT("UBits c = 0",   u.c == 0);
    ASSERT("UBits d = 0",   u.d == 0);

    ASSERT("SBits s1 = 0",  s.s1 == 0);
    ASSERT("SBits s3 = 0",  s.s3 == 0);
    ASSERT("SBits s7 = 0",  s.s7 == 0);

    ASSERT("SR carry = 0",  sr.carry    == 0);
    ASSERT("SR zero  = 0",  sr.zero     == 0);
}

/* 10. Toggle / bitmask operations */
static void test_toggle(void)
{
    SECTION("Toggle and bitmask operations");
    StatusReg sr = {0};

    /* Set all single-bit flags in a loop via an unsigned int alias */
    sr.carry    = 1;
    sr.zero     = 0;
    sr.overflow = 1;
    sr.negative = 0;

    /* XOR toggle using the field directly */
    sr.carry ^= 1;
    ASSERT("toggle carry 1→0",           sr.carry    == 0);
    sr.carry ^= 1;
    ASSERT("toggle carry 0→1",           sr.carry    == 1);

    sr.overflow ^= 1;
    ASSERT("toggle overflow 1→0",        sr.overflow == 0);
}

/* ─── Entry point ────────────────────────────────────────────────── */
int main(void)
{
    printf("C Bitfield Unit Tests\n");
    printf("=====================\n");

    test_basic_read_write();
    test_overflow_unsigned();
    test_signed_fields();
    test_mixed_struct();
    test_padding_field();
    test_status_register();
    test_ipv4_header();
    test_all_ones();
    test_zero_init();
    test_toggle();

    printf("\n=====================\n");
    printf("Results: %d / %d passed", tests_passed, tests_run);
    if (tests_passed == tests_run)
        printf("  ✓ All tests passed!\n");
    else
        printf("  ✗ %d test(s) FAILED.\n", tests_run - tests_passed);

    return (tests_passed == tests_run) ? 0 : 1;
}