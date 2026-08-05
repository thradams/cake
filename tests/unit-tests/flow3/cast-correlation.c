#pragma safety enable

/*
   A cast keeps correlation. Each source value alternative is cast on its own
   path (never just data[0]), keeping the branch origin that produced it, so a
   join survives the cast and the widths still fold.

   Use compile_assert (flow-checked), not static_assert (C11 compile-time).
*/

/* Widening cast preserves the correlated join: a is {1@then, 3@else}, so
   (long)a is {1, 3} -- the impossible cross value 2 stays excluded. */
void widen(int c)
{
    int a;
    if (c) a = 1; else a = 3;

    long la = (long)a;
    compile_assert(la == 1 || la == 3);
    compile_assert(la != 2);
}

/* Two correlated variables stay paired through casts of each. */
void paired(int c)
{
    int a, b;
    if (c) { a = 1; b = 2; } else { a = 3; b = 4; }

    long la = (long)a;
    long lb = (long)b;
    compile_assert(la + lb == 3 || la + lb == 7);   /* 5 excluded by correlation */
    compile_assert(la + lb != 5);
}

/* Narrowing cast truncates each alternative; the results still fold. */
void truncate(int c)
{
    int a;
    if (c) a = 300; else a = 301;                   /* both wrap mod 256 */

    unsigned char uc = (unsigned char)a;
    compile_assert(uc == 44 || uc == 45);           /* 300%256, 301%256 */
    compile_assert(uc != 0);
    /* NOT provable: uc == 44   (301 path gives 45) */
}
