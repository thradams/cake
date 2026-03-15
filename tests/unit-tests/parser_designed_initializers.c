
/* -----------------------------------------------------------------------
 * 1. Basic struct member designation
 * --------------------------------------------------------------------- */
void test_struct_basic(void)
{
    struct Point { int x; int y; };

    constexpr struct Point p = { .x = 10, .y = 20 };  // OK
    static_assert(p.x == 10);
    static_assert(p.y == 20);

    /* Only one member designated — the other must be zero-initialised */
    constexpr struct Point q = { .y = 5 };             // OK
    static_assert(q.x == 0);
    static_assert(q.y == 5);
}

/* -----------------------------------------------------------------------
 * 2. Out-of-order struct member designation
 * --------------------------------------------------------------------- */
void test_struct_out_of_order(void)
{
    struct RGB { unsigned char r, g, b; };

    constexpr struct RGB c = { .b = 255, .r = 128 };  // OK  (g == 0)
    static_assert(c.r == 128);
    static_assert(c.g == 0);
    static_assert(c.b == 255);
}

/* -----------------------------------------------------------------------
 * 3. Re-designation (last write wins)
 * --------------------------------------------------------------------- */
void test_struct_redesignation(void)
{
    struct Pair { int a; int b; };

    /* .a is written twice; the second value must survive */
    constexpr struct Pair p = { .a = 1, .b = 2, .a = 99 }; // OK
    static_assert(p.a == 99);
    static_assert(p.b == 2);
}

/* -----------------------------------------------------------------------
 * 4. Basic array index designation
 * --------------------------------------------------------------------- */
void test_array_basic(void)
{
    constexpr int a[5] = { [2] = 42 };                 // OK
    static_assert(a[0] == 0);
    static_assert(a[1] == 0);
    static_assert(a[2] == 42);
    static_assert(a[3] == 0);
    static_assert(a[4] == 0);
}

/* -----------------------------------------------------------------------
 * 5. Array designation at last valid index
 * --------------------------------------------------------------------- */
void test_array_last_index(void)
{
    constexpr int a[4] = { [3] = 7 };                  // OK
    static_assert(a[3] == 7);
    static_assert(a[0] == 0);
}

/* -----------------------------------------------------------------------
 * 6. Array designation with implicit continuation
 *    After a designator the following values fill successive positions.
 * --------------------------------------------------------------------- */
void test_array_continuation(void)
{
    constexpr int a[6] = { [2] = 10, 20, 30 };        // OK
    static_assert(a[0] == 0);
    static_assert(a[1] == 0);
    static_assert(a[2] == 10);
    static_assert(a[3] == 20);
    static_assert(a[4] == 30);
    static_assert(a[5] == 0);
}

/* -----------------------------------------------------------------------
 * 7. Array designation re-assigns an index (last write wins)
 * --------------------------------------------------------------------- */
void test_array_redesignation(void)
{
    constexpr int a[4] = { [1] = 5, [1] = 99 };       // OK
    static_assert(a[1] == 99);
}

/* -----------------------------------------------------------------------
 * 8. Unknown-size array — size inferred from highest designator + 1
 * --------------------------------------------------------------------- */
void test_array_size_inferred(void)
{
    constexpr int a[] = { [4] = 1 };                   // OK  size == 5
    static_assert(sizeof(a) / sizeof(a[0]) == 5);
    static_assert(a[4] == 1);
    static_assert(a[0] == 0);
}

/* -----------------------------------------------------------------------
 * 9. Nested struct designation (chained .member.member)
 * --------------------------------------------------------------------- */
void test_nested_struct(void)
{
    struct Inner { int val; };
    struct Outer { struct Inner inner; int x; };

    constexpr struct Outer o = { .inner.val = 7, .x = 3 }; // OK
    static_assert(o.inner.val == 7);
    static_assert(o.x == 3);
    static_assert(o.inner.val + o.x == 10);
}

/* -----------------------------------------------------------------------
 * 10. Struct containing a fixed char array — designated member
 *     sizeof is always a constant expression so no constexpr needed there,
 *     but the struct itself must be constexpr for member asserts.
 * --------------------------------------------------------------------- */
void test_struct_with_char_array(void)
{
    struct Named { int id; char text[8]; };

    constexpr struct Named n = { .id = 1, .text = {"hello"} }; // OK
    static_assert(n.id == 1);
    static_assert(sizeof(n.text) == 8);               // sizeof is always constant
    static_assert(n.text[0] == 'h');
    static_assert(n.text[5] == '\0');
}

/* -----------------------------------------------------------------------
 * 11. Union designation — only the named member is active
 * --------------------------------------------------------------------- */
void test_union_designation(void)
{
    union Data { int i; float f; unsigned char bytes[4]; };

    constexpr union Data d = { .i = 42 };              // OK
    static_assert(d.i == 42);

    constexpr union Data e = { .bytes = {0x01, 0x02} }; // OK
    static_assert(e.bytes[0] == 0x01);
    static_assert(e.bytes[1] == 0x02);
    static_assert(e.bytes[2] == 0x00);
}

/* -----------------------------------------------------------------------
 * 12. Anonymous struct inside union
 * --------------------------------------------------------------------- */
void test_anonymous_struct_in_union(void)
{
    union Vec2
    {
        struct { int x; int y; };
        int v[2];
    };

    constexpr union Vec2 u = { .x = 3, .y = 4 };      // OK
    static_assert(u.x == 3);
    static_assert(u.y == 4);
}

/* -----------------------------------------------------------------------
 * 13. constexpr struct — out-of-order designation
 * --------------------------------------------------------------------- */
void test_constexpr_struct(void)
{
    struct Rect { int w; int h; };

    constexpr struct Rect r = { .h = 480, .w = 640 }; // OK (out-of-order)
    static_assert(r.w == 640);
    static_assert(r.h == 480);
    static_assert(r.w * r.h == 307200);
}

/* -----------------------------------------------------------------------
 * 14. constexpr array with designated indices — size and values
 * --------------------------------------------------------------------- */
void test_constexpr_array(void)
{
    constexpr int lut[] = { [0] = 1, [1] = 2, [3] = 8 }; // OK, size == 4
    static_assert(sizeof(lut) / sizeof(lut[0]) == 4);
    static_assert(lut[0] == 1);
    static_assert(lut[1] == 2);
    static_assert(lut[2] == 0);                        // unspecified slot → 0
    static_assert(lut[3] == 8);
}

/* -----------------------------------------------------------------------
 * 15. Mixed designated and positional initializers
 * --------------------------------------------------------------------- */
void test_mixed_designated_positional(void)
{
    struct S { int a; int b; int c; };

    /* After .b = 10 the next positional slot is .c */
    constexpr struct S s = { 1, .b = 10, 20 };        // OK
    static_assert(s.a == 1);
    static_assert(s.b == 10);
    static_assert(s.c == 20);
}

/* -----------------------------------------------------------------------
 * 16. Array of structs with designated indices
 * --------------------------------------------------------------------- */
void test_array_of_structs(void)
{
    struct Pt { int x; int y; };

    constexpr struct Pt pts[4] = {
        [0] = { .x = 1, .y = 2 },
        [3] = { .x = 7, .y = 8 },
    };                                                  // OK
    static_assert(pts[0].x == 1);
    static_assert(pts[0].y == 2);
    static_assert(pts[1].x == 0);                      // unspecified → zero
    static_assert(pts[2].y == 0);                      // unspecified → zero
    static_assert(pts[3].x == 7);
    static_assert(pts[3].y == 8);
}

/* -----------------------------------------------------------------------
 * 17. Array designation at index 0 (lower boundary)
 * --------------------------------------------------------------------- */
void test_array_index_zero(void)
{
    constexpr int a[3] = { [0] = 99 };                 // OK
    static_assert(a[0] == 99);
    static_assert(a[1] == 0);
    static_assert(a[2] == 0);
}

/* -----------------------------------------------------------------------
 * ERROR CASES
 * Compile each function in isolation to verify the correct diagnostic.
 * --------------------------------------------------------------------- */

/* E1. Non-existent struct member — C_ERROR_STRUCT_MEMBER_NOT_FOUND */
void test_error_unknown_member(void)
{
    struct S { int x; };
    
    [[cake::e720]]
    constexpr struct S s = { .z = 1 };                 // C_ERROR_STRUCT_MEMBER_NOT_FOUND
}

/* E2. Array designator out of bounds (fixed-size) — C_ERROR_STRUCT_MEMBER_NOT_FOUND */
void test_error_array_index_oob(void)
{
    [[cake::e720]]
    constexpr int a[3] = { [5] = 1 };                  // C_ERROR_STRUCT_MEMBER_NOT_FOUND
}

/* E3. Negative array designator — C_ERROR_STRUCT_MEMBER_NOT_FOUND */
void test_error_negative_index(void)
{
    [[cake::e720]]
    constexpr int a[3] = { [-1] = 0 };                 // C_ERROR_STRUCT_MEMBER_NOT_FOUND
}

/* E4. Excess elements spill past end after a designator — W_TO_MANY_INITIALIZERS */
void test_warning_excess_after_designator(void)
{
    [[cake::w62]]
    constexpr int a[2] = { [1] = 1, 2 };               // W_TO_MANY_INITIALIZERS
}

/* E5. Designated char-array member: string too long — W_ARRAY_SIZE */
void test_warning_string_too_long_designated(void)
{
    struct Msg { int id; char text[4]; };
    [[cake::w52]]
    constexpr struct Msg m = { .id = 0, .text = {"toolong"} }; // W_ARRAY_SIZE
}