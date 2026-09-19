#pragma safety enable

/* relational folding considers every alternative: with a in {5, 7}, a > 3 and a < 10 are provable, a > 6 and a < 6 are not */

void provable(int cond)
{
    int a;
    if (cond) a = 5; else a = 7;

    compile_assert(a > 3);
    compile_assert(a < 10);
    compile_assert(a >= 5);
    compile_assert(a <= 7);

    /* not provable: a > 6 (5 fails), a < 6 (7 fails) */
}

/* Three-way join: a in {2, 4, 8}. */
void provable_three_way(int k)
{
    int a;
    if (k == 0) a = 2;
    else if (k == 1) a = 4;
    else a = 8;

    compile_assert(a > 1);   /* 2,4,8 all > 1 */
    compile_assert(a < 9);   /* all < 9 */
}
