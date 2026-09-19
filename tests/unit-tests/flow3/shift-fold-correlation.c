#pragma safety enable

/* << and >> fold per alternative keeping the branch origin, so a join survives the shift */

/* n is {1@then, 2@else}: 1<<n is {2, 4} */
void by_variable(int c)
{
    int n;
    if (c) n = 1; else n = 2;

    int v = 1 << n;
    compile_assert(v == 2 || v == 4);
    compile_assert(v != 3);
    /* NOT provable: v == 2   (else path gives 4) */
}

/* Right shift folds the same way. */
void right(int c)
{
    int a;
    if (c) a = 16; else a = 32;

    int v = a >> 2;
    compile_assert(v == 4 || v == 8);
    compile_assert(v != 6);
}

/* Out-of-range shift counts are treated as unknown (ANY), not folded. */
void out_of_range(int c)
{
    int n;
    if (c) n = 3; else n = 100;      /* 100 >= width -> unknown */

    int v = 1 << n;
    compile_assert(v == 8 || v != 8); /* trivially true: no false claim proven */
}

/* known limitation: `(1 << n) == 2 || (1 << n) == 4` re-folds each side independently and fails; bind `int v = 1 << n` first */
