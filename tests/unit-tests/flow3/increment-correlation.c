#pragma safety enable

/*
   ++ / -- keep correlation, and resolve the lvalue by iterating its REF
   alternatives (like an assignment destination) -- so they work even when the
   operand aliases several objects, e.g. `(*p)++`.

   Use compile_assert (flow-checked), not static_assert (C11 compile-time).
*/

/* A plain `x++` advances every value alternative, keeping its branch origin,
   so the join stays correlated. */
void simple(int c)
{
    int a, b;
    if (c) { a = 1; b = 2; } else { a = 3; b = 4; }
    a++;                              /* a: {1@then,3@else} -> {2@then,4@else} */
    compile_assert(a + b == 4 || a + b == 8);   /* correlated: 6 is excluded */
    compile_assert(a + b != 6);
}

/* `(*p)++` where p can point to a or b: the deref carries p's branch origin,
   and ++ iterates both REF targets, advancing each on its own path. */
void through_pointer(int c)
{
    int a = 1, b = 2, *p;
    if (c) { p = &a; } else { p = &b; }

    (*p)++;                           /* then: a->2 ; else: b->3 */

    compile_assert(*p == 2 || *p == 3);
    /* NOT provable (each has a counter-path):
         compile_assert(*p == 2);          // else path gives 3
         compile_assert(*p == 5 || *p == 3);
    */
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
