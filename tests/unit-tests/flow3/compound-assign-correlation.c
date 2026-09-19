#pragma safety enable

/* compound assignment folds per alternative, keeping each branch origin, so a correlated join survives it */

/* a is {1@then, 3@else}: after a += 10 it is {11, 13}, with b {13, 17}, never 15 */
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

/* pointer += never makes a valid pointer null, the non-null guarantee is kept */
void pointer(int* a)
{
    a += 1;
    *a = 0;                                  /* no possibly-null warning */
}
