#pragma safety enable

/* ++/-- keep correlation and iterate the lvalue's REF alternatives, so `(*p)++` works when p aliases several objects */

/* x++ advances every alternative keeping its branch origin */
void simple(int c)
{
    int a, b;
    if (c) { a = 1; b = 2; } else { a = 3; b = 4; }
    a++;                              /* a: {1@then,3@else} -> {2@then,4@else} */
    compile_assert(a + b == 4 || a + b == 8);   /* correlated: 6 is excluded */
    compile_assert(a + b != 6);
}

/* (*p)++ with p pointing to a or b advances each target on its own path */
void through_pointer(int c)
{
    int a = 1, b = 2, *p;
    if (c) { p = &a; } else { p = &b; }

    (*p)++;                           /* then: a->2 ; else: b->3 */

    compile_assert(*p == 2 || *p == 3);
    /* not provable: *p == 2 (else path gives 3), *p == 5 || *p == 3 */
}

/* Prefix and decrement behave the same way. */
void prefix_and_dec(int c)
{
    int x;
    if (c) x = 10; else x = 20;
    --x;                              /* {9@then, 19@else} */
    compile_assert(x == 9 || x == 19);
    compile_assert(x != 10);
}
