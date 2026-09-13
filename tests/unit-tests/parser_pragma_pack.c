/*
 * #pragma pack - member alignment capped at n bytes for the structs declared
 * while it is in effect. Expected values were measured with clang on
 * macos_arm64 / x86-64 (GCC agrees); the MSVC-specific ones are marked.
 */

/*
 * 1. push/pop, and that the struct keeps the value it was declared under
 */
#pragma pack(push, 1)
struct P1 { char c; int i; };
#pragma pack(pop)

struct P1_after { char c; int i; };

static_assert(sizeof(struct P1) == 5, "pack(1): no padding");
static_assert(_Alignof(struct P1) == 1, "pack(1): struct alignment capped");
static_assert(offsetof(struct P1, i) == 1, "pack(1): i right after c");
static_assert(sizeof(struct P1_after) == 8, "pop restores the default");
static_assert(_Alignof(struct P1_after) == 4, "pop restores the default");

/*
 * 2. pack(n) without push, and pack() reset
 */
#pragma pack(2)
struct P2 { char c; int i; double d; };
#pragma pack()
struct P2_after { char c; int i; double d; };

static_assert(sizeof(struct P2) == 14, "pack(2): int at 2, double at 6");
static_assert(_Alignof(struct P2) == 2, "pack(2)");
static_assert(offsetof(struct P2, d) == 6, "pack(2): double capped to 2");
static_assert(sizeof(struct P2_after) == 16, "pack() resets");

/*
 * 3. A pack value larger than the natural alignment changes nothing
 */
#pragma pack(push, 16)
struct P3 { char c; int i; };
#pragma pack(pop)
static_assert(sizeof(struct P3) == 8, "pack(16) is not a minimum");
static_assert(_Alignof(struct P3) == 4, "pack(16) is not a minimum");

/*
 * 4. Nested push: inner value, then back to the outer one
 */
#pragma pack(push, 4)
struct P4_outer { char c; double d; };
#pragma pack(push, 1)
struct P4_inner { char c; double d; };
#pragma pack(pop)
struct P4_outer2 { char c; double d; };
#pragma pack(pop)

static_assert(sizeof(struct P4_outer) == 12, "pack(4): double at 4");
static_assert(sizeof(struct P4_inner) == 9, "pack(1) inside pack(4)");
static_assert(sizeof(struct P4_outer2) == 12, "pop returns to pack(4)");

/*
 * 5. A packed struct used as a member of a normal one keeps its own layout
 */
#pragma pack(push, 1)
struct P5_in { char c; int i; };
#pragma pack(pop)
struct P5 { char c; struct P5_in in; int i; };

static_assert(sizeof(struct P5_in) == 5, "");
static_assert(offsetof(struct P5, in) == 1, "packed member has alignment 1");
static_assert(offsetof(struct P5, i) == 8, "int member keeps alignment 4");
static_assert(sizeof(struct P5) == 12, "");

/*
 * 6. Typedef'd anonymous struct and a union
 */
#pragma pack(push, 1)
typedef struct { char c; short s; long long ll; } P6_t;
union P6_u { char c; int i; };
#pragma pack(pop)

static_assert(sizeof(P6_t) == 11, "");
static_assert(_Alignof(P6_t) == 1, "");
static_assert(sizeof(union P6_u) == 4, "");
static_assert(_Alignof(union P6_u) == 1, "");

/*
 * 7. Bit-fields under #pragma pack
 *
 * GCC/Clang: a bit-field goes at the next free bit; the usual rule that it
 * must not straddle a storage unit of its type is dropped while any pack
 * value is in effect. A zero-width bit-field still aligns to its type's
 * natural alignment. MSVC keeps its storage-unit model, with the unit's
 * alignment capped.
 */
#pragma pack(push, 1)
struct B1 { char a; int b : 20; char c; };
struct B2 { unsigned a : 7; unsigned b : 30; };
struct B3 { char a : 3; int b : 9; };
struct B4 { char a; short b : 9; char c; };
struct B5 { int a : 3; int : 0; int b : 3; };
struct B6 { long long a : 40; char b; };
struct B7 { char a; int b : 30; char c; };
struct B8 { char a : 5; int b : 12; char c; };
struct B9 { char a; short b : 15; short c : 15; };
struct B10 { char a; int : 0; char b; };
#pragma pack(pop)

#pragma pack(push, 2)
struct B11 { char a; int b : 30; char c; };
struct B12 { char a; int b : 12; char c; };
struct B13 { char a; char b : 7; char c : 7; };
#pragma pack(pop)

#pragma pack(push, 4)
struct B14 { char a; int b : 30; char c; };
struct B15 { char a; long long b : 60; char c; };
#pragma pack(pop)

#pragma pack(push, 8)
struct B16 { char a; int b : 30; char c; };
#pragma pack(pop)

#ifdef _MSC_VER
static_assert(sizeof(struct B1) == 6, "MSVC: char unit, int unit at 1, char at 5");
static_assert(_Alignof(struct B1) == 1, "");
static_assert(sizeof(struct B2) == 8, "MSVC: 37 bits need two int units");
static_assert(sizeof(struct B3) == 5, "MSVC: char unit + int unit");
#else
static_assert(sizeof(struct B1) == 5, "b at bits 8..27, c at byte 4");
static_assert(_Alignof(struct B1) == 1, "");
static_assert(offsetof(struct B1, c) == 4, "");
static_assert(sizeof(struct B2) == 5, "37 bits, straddling allowed");
static_assert(sizeof(struct B3) == 2, "12 bits");
static_assert(sizeof(struct B4) == 4, "");
static_assert(sizeof(struct B5) == 5, ":0 still aligns to int");
static_assert(_Alignof(struct B5) == 1, "");
static_assert(sizeof(struct B6) == 6, "");
static_assert(sizeof(struct B7) == 6, "b at bits 8..37, c at byte 5");
static_assert(offsetof(struct B7, c) == 5, "");
static_assert(sizeof(struct B8) == 4, "");
static_assert(sizeof(struct B9) == 5, "38 bits");
static_assert(sizeof(struct B10) == 5, "");
static_assert(offsetof(struct B10, b) == 4, ":0 aligns b to int");

static_assert(sizeof(struct B11) == 6, "pack(2)");
static_assert(_Alignof(struct B11) == 2, "pack(2): int bit-field contributes 2");
static_assert(sizeof(struct B12) == 4, "pack(2): b at bit 8, no alignment to 2");
static_assert(sizeof(struct B13) == 3, "pack(2): 22 bits");
static_assert(_Alignof(struct B13) == 1, "");

static_assert(sizeof(struct B14) == 8, "pack(4): straddling allowed, then padded to 4");
static_assert(_Alignof(struct B14) == 4, "");
static_assert(sizeof(struct B15) == 12, "pack(4): long long capped to 4");
static_assert(_Alignof(struct B15) == 4, "");

static_assert(sizeof(struct B16) == 8, "pack(8): straddling still allowed");
static_assert(_Alignof(struct B16) == 4, "");
#endif

int main(void)
{
    return 0;
}
