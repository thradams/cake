/* static_assert_operators.c
   Compile-time unit tests demonstrating many C operators used
   inside integer constant expressions (_Static_assert).
   Target: C11 or later
*/

typedef typeof(sizeof(0)) size_t;
#define offsetof(A, B) __builtin_offsetof(A, B)

/* ---------- Arithmetic operators ---------- */
_Static_assert(1 + 2 == 3, "addition + failed");
_Static_assert(5 - 2 == 3, "subtraction - failed");
_Static_assert(2 * 3 == 6, "multiplication * failed");
_Static_assert(8 / 4 == 2, "division / failed");
_Static_assert(7 % 4 == 3, "modulo % failed");

/* ---------- Unary operators ---------- */
_Static_assert(+3 == 3, "unary + failed");
_Static_assert(-3 == -3, "unary - failed");
_Static_assert(!0 == 1, "logical not ! failed");    /* ! */
_Static_assert(!1 == 0, "logical not ! (1) failed");
//_Static_assert(~0 == (intptr_t)-1, "bitwise not ~ failed"); /* ~ (note: cast for signed comparison) */

/* ---------- Relational and equality ---------- */
_Static_assert(3 < 4, "< operator failed");
_Static_assert(3 <= 3, "<= operator failed");
_Static_assert(5 > 2, "> operator failed");
_Static_assert(5 >= 5, ">= operator failed");
_Static_assert(2 == 2, "== operator failed");
_Static_assert(2 != 3, "!= operator failed");

/* ---------- Logical (short-circuit style) ---------- */
/* In constant expressions, both sides are constant; short-circuit is not meaningful here,
   but we still test logical operators' results. */
_Static_assert((1 && 1) == 1, "logical && failed");
_Static_assert((1 && 0) == 0, "logical && false case failed");
_Static_assert((0 || 0) == 0, "logical || all false failed");
_Static_assert((0 || 5) == 1, "logical || true case failed"); /* nonzero -> true */

/* ---------- Bitwise operators ---------- */
_Static_assert((5 & 3) == 1, "bitwise & failed");
_Static_assert((5 | 2) == 7, "bitwise | failed");
_Static_assert((5 ^ 1) == 4, "bitwise ^ failed");
_Static_assert((1 << 3) == 8, "left shift << failed");
_Static_assert((16 >> 2) == 4, "right shift >> failed");

/* ---------- Conditional (?:) ---------- */
_Static_assert((1 ? 2 : 3) == 2, "ternary true failed");
_Static_assert((0 ? 2 : 3) == 3, "ternary false failed");

/* ---------- Comma operator (evaluates left to right, returns last) ---------- */
[[cake::w0960]]
_Static_assert(((1, 2, 3)) == 3, "comma operator failed");
//warning in clang error in gcc, error in cake

/* ---------- sizeof and _Alignof ---------- */
_Static_assert(sizeof(char) == 1, "sizeof(char) must be 1 by standard");
_Static_assert((size_t) sizeof(int) >= 2, "sizeof(int) is at least 2 (implementation-dependent)"); /* weak sanity check */
_Static_assert(_Alignof(char) == 1, "_Alignof(char) must be 1");

/* ---------- casts and integer promotions ---------- */
_Static_assert((int) (3u) == 3, "cast unsigned->int failed");
_Static_assert(((unsigned) 5) - ((unsigned) 2) == 3u, "unsigned arithmetic/cast failed");

/* ---------- character constants and arithmetic ---------- */
_Static_assert(('a' + 1) == 'b', "char arithmetic failed");
_Static_assert(('Z' - 'A') == 25, "char difference failed");

/* ---------- use of enums in constant expressions ---------- */
enum E {
    E_ZERO = 0,
    E_ONE = 1,
    E_THREE = (1 + 2)
};
_Static_assert(E_THREE == 3, "enum computation failed");

/* ---------- offsetof (from stddef.h) ---------- */
struct S {
    int a;
    char b;
    long long c;
};

/* On typical implementations, b immediately follows a possibly with padding.
   We assert that offsetof is at least the size of a (a weak but safe check). */
   //_Static_assert(offsetof(struct S, b) >= sizeof(((struct S *)0)->a),
   //               "offsetof(struct S, b) should be >= sizeof int");

   /* ---------- array subscript in constant context (string literal) ---------- */
   /* The standard treats string literals as arrays of char; indexing a string literal in
      a constant expression is allowed if the characters are constant. */
_Static_assert("hi"[0] == 'h', "string literal subscript [0] failed");
_Static_assert("hi"[1] == 'i', "string literal subscript [1] failed");

/* ---------- pointer-to-integer: allowed only in some constant expressions.
   We show a compile-time expression using null pointer comparison (constant). */
_Static_assert((void*) 0 == (void*) 0, "null pointer equality failed");

/* ---------- combined complex expression using many operators ---------- */
_Static_assert(
    ((((((((1 + 2) * 3) >> 1) & 7) ^ 2) | 8) % 5) == 4),
    "complex combined expression failed"
);

/* ---------- Notes about operators NOT used here ---------- */
/*
 - Assignment operators (=, +=, -=, etc.) are not valid in constant expressions and thus
   cannot appear inside _Static_assert expressions.
 - Pre/post increment and decrement (++/--) cannot appear in constant expressions.
 - Function calls and most uses of unary & (address-of) on non-constant objects cannot be
   used in integer constant expressions.
 - Member access operators (.) and (->) are not used as constant-expression operands here,
   because they require object expressions that are not compile-time constants in general.
*/

/* If file compiles, most asserted operators behave as the tests expect at compile-time. */
int main(void) { return 0; }
