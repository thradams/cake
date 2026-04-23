
/*
 * 1. BASIC PACKING - bits from the same type share one storage unit
 *
 * Three fields totalling 13 bits all fit inside one 32-bit int unit.
 * Both targets agree.
 *  */
struct Basic {
    unsigned int a : 5; /* bits  0-4  */
    unsigned int b : 4; /* bits  5-8  */
    unsigned int c : 4; /* bits  9-12 */
};
static_assert(sizeof(struct Basic) == 4, "Basic: 13 bits fit in one int unit");
static_assert(alignof(struct Basic) == 4, "Basic: aligned to int (4 bytes)");

/* 1. Basic - all three bitfields named explicitly */
constexpr struct Basic basic = {.a = 31, .b = 15, .c = 7};
static_assert(basic.a == 31); /* 0x1F - max for 5 bits */
static_assert(basic.b == 15); /* 0x0F - max for 4 bits */
static_assert(basic.c == 7);  /* 0x07 - max for 4 bits */


/*
 * 2. SPILL TO NEXT UNIT - bits that overflow start a fresh storage unit
 *
 * a+b = 30 bits in first int; c = 10 bits does not fit -> new int.
 * Both targets agree.
 *  */
struct Spill {
    unsigned int a : 15;
    unsigned int b : 15; /* a+b = 30 bits, still inside one int */
    unsigned int c : 10; /* 30+10 = 40 > 32 -> spills to second int */
};
static_assert(sizeof(struct Spill) == 8, "Spill: two int storage units");
static_assert(alignof(struct Spill) == 4, "Spill: aligned to int");

/* 2. Spill - only c named; a and b are zero-initialized */
constexpr struct Spill spill = {.c = 1023};
static_assert(spill.a == 0);
static_assert(spill.b == 0);
static_assert(spill.c == 1023); /* max for 10 bits */

/*
 * 3. ZERO-WIDTH UNNAMED BITFIELD - forces flush to next storage unit
 *
 * "unsigned int : 0" between a and b forces b into a new int unit.
 * Both targets agree.
 *  */
struct ZeroWidth {
    unsigned int a : 10;
    unsigned int : 0; /* flush - b must start at the next int boundary */
    unsigned int b : 10;
};
static_assert(sizeof(struct ZeroWidth) == 8, "ZeroWidth: :0 flushes storage unit");
static_assert(alignof(struct ZeroWidth) == 4, "ZeroWidth: aligned to int");

/* 3. ZeroWidth - a and b are in separate storage units */
constexpr struct ZeroWidth zw = {.a = 511, .b = 1};
static_assert(zw.a == 511); /* max for 10 bits */
static_assert(zw.b == 1);

/*
 * 4. UNNAMED (PADDING) BITFIELD - consumes bits, contributes no name
 *
 * 5 + 3(pad) + 8 = 16 bits -> one short unit exactly fills.
 * Both targets agree.
 *  */
struct Padding {
    unsigned short x : 5;
    unsigned short : 3;   /* 3 padding bits - still in same short unit */
    unsigned short y : 8; /* 5+3+8 = 16, exactly fills the short */
};
static_assert(sizeof(struct Padding) == 2, "Padding: 16 bits fit in one short");
static_assert(alignof(struct Padding) == 2, "Padding: aligned to short");

/* 4. Padding - only named fields; unnamed padding bits are untouched */
constexpr struct Padding pad = {.x = 31, .y = 255};
static_assert(pad.x == 31);  /* max for 5 bits */
static_assert(pad.y == 255); /* max for 8 bits */

/*
 * 5a. MIXED TYPE SIZES - char:3 + int:9
 *
 * GCC cross-type merge rule: when a new bitfield overflows the current
 * storage unit AND the new field's declared type is LARGER, GCC checks
 * whether ALL accumulated bits (previous + incoming) fit inside the larger
 * type. If they do, the previous content is absorbed into the new unit
 * without being flushed as a separate byte.
 *
 * Here: char:3 opens an 8-bit unit. int:9 overflows (3+9=12 > 8).
 * storage_bits(int=32) > storage_bits(char=8) and 3+9=12 <= 32 -> MERGE.
 * Both fields share one int unit.
 *
 * Layout (GCC / TARGET_X86_X64_GCC):
 *   offset 0: int unit (4 bytes) - holds a (3 bits) and b (9 bits)
 *   total: 4 bytes
 *
 * MSVC rule: any change in storage-unit type size forces a new unit,
 * even when the bits would still fit.
 *
 * Layout (MSVC / TARGET_X86_MSVC, TARGET_X64_MSVC):
 *   offset 0: char unit (1 byte) - holds a
 *   offset 1-3: alignment padding
 *   offset 4: int unit  (4 bytes) - holds b
 *   total: 8 bytes
 *  */
struct MixedTypes {
    unsigned char a : 3;
    unsigned int b : 9;
};
#ifdef _MSC_VER
static_assert(sizeof(struct MixedTypes) == 8, "MixedTypes MSVC: char unit + int unit = 8");
static_assert(alignof(struct MixedTypes) == 4, "MixedTypes MSVC: alignof int = 4");
#else
static_assert(sizeof(struct MixedTypes) == 4, "MixedTypes GCC: merged into one int unit = 4");
static_assert(alignof(struct MixedTypes) == 4, "MixedTypes GCC: alignof int = 4");
#endif

/* 5a. MixedTypes */
constexpr struct MixedTypes mt = {.a = 7, .b = 511};
static_assert(mt.a == 7);   /* max for 3 bits */
static_assert(mt.b == 511); /* max for 9 bits */



/*
 * 5b. Cross-type merge: char:5 + int:5
 *
 * 5+5=10 bits fit in a 32-bit int.
 * GCC merges both into one int unit: size = 4.
 * MSVC opens two separate units: size = 8.
 *  */
struct CharInt_5_5 {
    unsigned char a : 5;
    unsigned int b : 5;
};
#ifdef _MSC_VER
static_assert(sizeof(struct CharInt_5_5) == 8, "CharInt_5_5 MSVC: two units = 8");
static_assert(alignof(struct CharInt_5_5) == 4, "CharInt_5_5 MSVC: alignof int = 4");
#else
static_assert(sizeof(struct CharInt_5_5) == 4, "CharInt_5_5 GCC: merged into one int unit = 4");
static_assert(alignof(struct CharInt_5_5) == 4, "CharInt_5_5 GCC: alignof int = 4");
#endif

/* 5b. CharInt_5_5 */
constexpr struct CharInt_5_5 ci = {.a = 31, .b = 31};
static_assert(ci.a == 31);
static_assert(ci.b == 31);


/*
 * 5c. Cross-type merge does NOT apply when bits do not fit
 *
 * char:3 + int:30 = 33 bits > 32: cannot merge into int.
 * GCC flushes the char unit and opens a fresh int unit.
 * Both targets agree: size = 8.
 *
 * Layout:
 *   offset 0: char unit (1 byte) - holds a
 *   offset 1-3: alignment padding
 *   offset 4: int unit  (4 bytes) - holds b
 *   total: 8 bytes
 *  */
struct Overflow_int {
    unsigned char a : 3;
    unsigned int b : 30;
};
static_assert(sizeof(struct Overflow_int) == 8, "Overflow_int: 33 bits > 32, no merge possible");
static_assert(alignof(struct Overflow_int) == 4, "Overflow_int: alignof int = 4");

/*
 * 5d. Cross-type merge with short: char:3 + short:5
 *
 * 3+5=8 bits fit in a 16-bit short.
 * GCC merges into one short unit: size = 2.
 * MSVC opens a new short unit (type size changed 1->2): size = 4.
 *  */
struct CharShort {
    unsigned char a : 3;
    unsigned short b : 5;
};
#ifdef _MSC_VER
static_assert(sizeof(struct CharShort) == 4, "CharShort MSVC: char unit + short unit = 4");
static_assert(alignof(struct CharShort) == 2, "CharShort MSVC: alignof short = 2");
#else
static_assert(sizeof(struct CharShort) == 2, "CharShort GCC: 8 bits merged into one short = 2");
static_assert(alignof(struct CharShort) == 2, "CharShort GCC: alignof short = 2");
#endif

/*
 * 5e. Multi-step merge: char:4 + char:4 + int:8
 *
 * char:4 + char:4 = 8 bits fill one char unit. int:8 overflows (8+8=16>8),
 * storage_bits(int=32) > storage_bits(char=8), 8+8=16 <= 32 -> GCC merges
 * all three fields into one int unit: size = 4.
 * MSVC: type change at c forces a new int unit: size = 8.
 *  */
struct CC_I {
    unsigned char a : 4;
    unsigned char b : 4;
    unsigned int c : 8;
};
#ifdef _MSC_VER
static_assert(sizeof(struct CC_I) == 8, "CC_I MSVC: char unit + int unit = 8");
static_assert(alignof(struct CC_I) == 4, "CC_I MSVC: alignof int = 4");
#else
static_assert(sizeof(struct CC_I) == 4, "CC_I GCC: all merged into one int unit = 4");
static_assert(alignof(struct CC_I) == 4, "CC_I GCC: alignof int = 4");
#endif

/* 5c. Overflow_int - only b named */
constexpr struct Overflow_int ov = {.b = 1073741823};
static_assert(ov.a == 0);
static_assert(ov.b == 1073741823); /* max for 30 bits */

/* 5d. CharShort */
constexpr struct CharShort cs = {.a = 7, .b = 31};
static_assert(cs.a == 7);  /* max for 3 bits */
static_assert(cs.b == 31); /* max for 5 bits */

/* 5e. CC_I */
constexpr struct CC_I cci = {.a = 15, .b = 15, .c = 255};
static_assert(cci.a == 15); /* max for 4 bits */
static_assert(cci.b == 15);
static_assert(cci.c == 255); /* max for 8 bits */




/*
 * 6. NORMAL MEMBER FLUSHES OPEN BITFIELD UNIT
 *
 *   offset 0: char unit (1 byte) - holds flags
 *   offset 1-3: padding to align int
 *   offset 4: int value (4 bytes)
 *   total: 8 bytes
 * Both targets agree.
 *  */
struct FlushOnNormal {
    unsigned char flags : 4;
    int value;
};
static_assert(sizeof(struct FlushOnNormal) == 8, "FlushOnNormal: bitfield unit flushed before int");
static_assert(alignof(struct FlushOnNormal) == 4, "FlushOnNormal: alignof is 4");

/* 6. FlushOnNormal - mix of bitfield and normal member */
constexpr struct FlushOnNormal fn = {.flags = 15, .value = -1};
static_assert(fn.flags == 15); /* max for 4 bits */
static_assert(fn.value == -1);


/*
 * 7. BITFIELD AFTER NORMAL MEMBER
 *
 *   offset 0: int x     (4 bytes)
 *   offset 4: int unit  (4 bytes) - holds flags
 *   total: 8 bytes
 * Both targets agree.
 *  */
struct NormalThenBitfield {
    int x;
    unsigned int flags : 3;
};
static_assert(sizeof(struct NormalThenBitfield) == 8, "NormalThenBitfield: int then bitfield int unit");
static_assert(alignof(struct NormalThenBitfield) == 4, "NormalThenBitfield: alignof 4");

/* 7. NormalThenBitfield - normal member first, then bitfield */
constexpr struct NormalThenBitfield ntb = {.x = 42, .flags = 7};
static_assert(ntb.x == 42);
static_assert(ntb.flags == 7); /* max for 3 bits */


/*
 * 8. EXACT FIT - 32 bits filling exactly one int unit, no tail padding
 * Both targets agree.
 *  */
struct ExactFit {
    unsigned int a : 8;
    unsigned int b : 8;
    unsigned int c : 8;
    unsigned int d : 8;
};
static_assert(sizeof(struct ExactFit) == 4, "ExactFit: 32 bits = exactly one int");
static_assert(alignof(struct ExactFit) == 4, "ExactFit: aligned to int");


/* 8. ExactFit - all four bytes of the int unit named */
constexpr struct ExactFit ef = {.a = 0xAA, .b = 0xBB, .c = 0xCC, .d = 0xDD};
static_assert(ef.a == 0xAA);
static_assert(ef.b == 0xBB);
static_assert(ef.c == 0xCC);
static_assert(ef.d == 0xDD);


/*
 * 9a. UNION WITH BITFIELDS ONLY - size vs alignment divergence
 *
 * sizeof: both targets give 4 (one int storage unit).
 *
 * alignof:
 *   GCC  (TARGET_X86_X64_GCC): 4 - declared type drives alignment.
 *   MSVC (TARGET_X86_MSVC, TARGET_X64_MSVC): 1 - bitfield members do NOT
 *     contribute to alignment; only non-bitfield members do. A union whose
 *     members are all bitfields gets alignof == 1.
 *  */
union BitUnion {
    unsigned int lo : 16;
    unsigned int hi : 16;
};
static_assert(sizeof(union BitUnion) == 4, "BitUnion: one int storage unit = 4 bytes");
#ifdef _MSC_VER
static_assert(alignof(union BitUnion) == 1, "BitUnion MSVC: all-bitfield union alignof = 1");
#else
static_assert(alignof(union BitUnion) == 4, "BitUnion GCC: alignof == declared type (4)");
#endif

/*
 * 9b. UNION - non-bitfield member restores normal alignment on MSVC
 *
 * Adding one normal (non-bitfield) member makes MSVC use its alignment.
 * Both targets agree: alignof == 4.
 *  */
union BitUnion_mixed {
    unsigned int a : 16; /* bitfield - ignored by MSVC for alignment */
    unsigned int b;      /* normal member - counts on both targets    */
};
static_assert(sizeof(union BitUnion_mixed) == 4, "BitUnion_mixed: size 4");
static_assert(alignof(union BitUnion_mixed) == 4, "BitUnion_mixed: non-BF member drives align = 4");

/* 9a. BitUnion - only one member active at a time (union semantics) */
constexpr union BitUnion bu = {.lo = 0xABCD};
static_assert(bu.lo == 0xABCD);

/* 9b. BitUnion_mixed - initialize via the normal member */
constexpr union BitUnion_mixed bum = {.b = 0x12345678};
static_assert(bum.b == 0x12345678);



/*
 * 10. TAIL PADDING - struct padded to its alignment at the end
 *
 *   offset 0: char unit (1 byte) - holds a
 *   offset 1: 1 byte padding     - to align short
 *   offset 2: short b (2 bytes)
 *   total: 4 bytes
 * Both targets agree.
 *  */
struct TailPad {
    unsigned char a : 3;
    short b;
};
static_assert(sizeof(struct TailPad) == 4, "TailPad: char unit + short with padding = 4");
static_assert(alignof(struct TailPad) == 2, "TailPad: alignof = short = 2");

/* 10. TailPad - bitfield then normal short */
constexpr struct TailPad tp = {.a = 5, .b = 1000};
static_assert(tp.a == 5); /* fits in 3 bits */
static_assert(tp.b == 1000);


/*
 * 11. SINGLE-BIT _Bool BITFIELDS
 * Both targets agree.
 *  */
struct Flags {
    _Bool active : 1;
    _Bool visible : 1;
    _Bool locked : 1;
};
static_assert(sizeof(struct Flags) == 1, "Flags: three 1-bit _Bool fields in one byte");
static_assert(alignof(struct Flags) == 1, "Flags: _Bool alignment = 1");

/* Partial initializer: only locked set, others zero */
constexpr struct Flags flags2 = {.locked = 1};
static_assert(flags2.active == 0);
static_assert(flags2.visible == 0);
static_assert(flags2.locked == 1);


/* 11. Flags - three independent 1-bit booleans */
constexpr struct Flags flags = {.active = 1, .visible = 1, .locked = 0};
static_assert(flags.active == 1);
static_assert(flags.visible == 1);
static_assert(flags.locked == 0);

int main() {}
