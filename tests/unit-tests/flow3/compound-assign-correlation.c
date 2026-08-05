#pragma safety enable

/*
   Compound assignment (+=, -=, *=, ...) folds per alternative, so a correlated
   join survives it. Each value is advanced on its own path, keeping the branch
   origin -- never just data[0] -- so cross-branch values stay excluded.

   Use compile_assert (flow-checked), not static_assert (C11 compile-time).
*/

/* += on a correlated value keeps the pairing: a is {1@then, 3@else}, so after
   a += 10 it is {11, 13}; combined with b it is {13, 17}, never 15. */
void add(int c)
{
    int a, b;
    if (c) { a = 1; b = 2; } else { a = 3; b = 4; }

    a += 10;                                 /* {11@then, 13@else} */
    compile_assert(a + b == 13 || a + b == 17);
    compile_assert(a + b != 15);
}

/* Several forms, each folded per path. */
void mixed(int c)
{
    int x;
    if (c) x = 10; else x = 20;

    x *= 3;                                  /* {30, 60} */
    compile_assert(x == 30 || x == 60);

    x -= 5;                                  /* {25, 55} */
    compile_assert(x == 25 || x == 55);
    compile_assert(x != 40);
}

/* Pointer += is pointer arithmetic: it can never turn a valid pointer into a
   null one, so a non-optional pointer keeps its non-null guarantee. */
void pointer(int* a)
{
    a += 1;
    *a = 0;                                  /* no possibly-null warning */
}
