/*
 * __attribute__((packed)) and __attribute__((aligned(n))) on structs and on
 * members. Expected values measured with clang (GCC agrees).
 *
 * packed on a struct caps every member at alignment 1 - the same layout as
 * #pragma pack(1), and emitted that way. packed on a member gives that one
 * member alignment 1. aligned(n) is a minimum: it raises the alignment of a
 * struct (and pads its size) or of a member, never lowers it.
 *
 * MSVC does not understand GNU __attribute__ syntax and has no equivalent
 * for per-member packed/aligned, so under MSVC we neutralize the attribute
 * (so the code at least parses) and skip the layout-dependent checks below,
 * which assume GCC/Clang semantics.
 */

#if defined(_MSC_VER) && !defined(__clang__)
  #define __attribute__(x)
#endif

#include <stddef.h>
#include <assert.h>

#ifndef _MSC_VER

/* struct-level packed, every spelling */
struct __attribute__((packed)) A1 { char c; int i; };
struct A2 { char c; int i; } __attribute__((packed));
struct __attribute__((__packed__)) A3 { char c; short s; long long ll; };
typedef struct { char c; int i; } __attribute__((packed)) A16_t;
union __attribute__((packed)) U1 { char c; int i; };

static_assert(sizeof(struct A1) == 5, ""); static_assert(_Alignof(struct A1) == 1, "");
static_assert(sizeof(struct A2) == 5, ""); static_assert(_Alignof(struct A2) == 1, "");
static_assert(sizeof(struct A3) == 11, ""); static_assert(_Alignof(struct A3) == 1, "");
static_assert(sizeof(A16_t) == 5, ""); static_assert(_Alignof(A16_t) == 1, "");
static_assert(sizeof(union U1) == 4, ""); static_assert(_Alignof(union U1) == 1, "");

/* member-level packed: before the type, after the type, after the name */
struct A4 { char c; int i __attribute__((packed)); short s; };
struct A5 { char c; __attribute__((packed)) int i; int j; };
struct A6 { char c; int __attribute__((packed)) i, j; };

static_assert(sizeof(struct A4) == 8, "i at 1, s at 6"); static_assert(_Alignof(struct A4) == 2, "");
static_assert(offsetof(struct A4, s) == 6, "");
static_assert(sizeof(struct A5) == 12, "i at 1, j at 8"); static_assert(_Alignof(struct A5) == 4, "");
static_assert(offsetof(struct A5, j) == 8, "");
static_assert(sizeof(struct A6) == 9, "both packed"); static_assert(_Alignof(struct A6) == 1, "");

/* struct-level aligned */
struct __attribute__((aligned(16))) A7 { char c; };
struct A8 { char c; } __attribute__((aligned(8)));
struct __attribute__((aligned)) A12 { char c; };
struct __attribute__((packed, aligned(4))) A11 { char c; int i; };

static_assert(sizeof(struct A7) == 16, ""); static_assert(_Alignof(struct A7) == 16, "");
static_assert(sizeof(struct A8) == 8, ""); static_assert(_Alignof(struct A8) == 8, "");
static_assert(sizeof(struct A12) == 16, "aligned with no argument: 16"); static_assert(_Alignof(struct A12) == 16, "");
static_assert(sizeof(struct A11) == 8, "packed members, struct padded to 4"); static_assert(_Alignof(struct A11) == 4, "");

/* member-level aligned, and that it cannot lower the natural alignment */
struct A9 { char c; char d __attribute__((aligned(4))); };
struct A10 { char c; int i __attribute__((aligned(2))); };
struct A13 { char c; char buf[3] __attribute__((aligned(16))); };
struct A17 { char c; _Alignas(8) int i; };
struct A18 { char c; _Alignas(4) char d; };

static_assert(sizeof(struct A9) == 8, ""); static_assert(_Alignof(struct A9) == 4, "");
static_assert(offsetof(struct A9, d) == 4, "");
static_assert(sizeof(struct A10) == 8, "aligned(2) on int stays 4"); static_assert(_Alignof(struct A10) == 4, "");
static_assert(sizeof(struct A13) == 32, ""); static_assert(_Alignof(struct A13) == 16, "");
static_assert(offsetof(struct A13, buf) == 16, "");
static_assert(sizeof(struct A17) == 16, ""); static_assert(_Alignof(struct A17) == 8, "");
static_assert(sizeof(struct A18) == 8, ""); static_assert(_Alignof(struct A18) == 4, "");

/* packed struct with bit-fields, and a packed struct as a member */
struct __attribute__((packed)) A14 { char a; int b : 20; char c; };
struct A15 { char c; struct A1 in; int i; };

static_assert(sizeof(struct A14) == 5, "b at bits 8..27, c at 4");
static_assert(sizeof(struct A15) == 12, ""); static_assert(_Alignof(struct A15) == 4, "");
static_assert(offsetof(struct A15, in) == 1, "");
static_assert(offsetof(struct A15, i) == 8, "");

#endif /* !_MSC_VER */

int main(void)
{
    return 0;
}
