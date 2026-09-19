#pragma safety enable

/* a cast is applied per alternative, keeping each one's branch origin, so joins survive it */

/* widening keeps the join: a is {1@then, 3@else}, so (long)a is {1, 3}, never 2 */
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
