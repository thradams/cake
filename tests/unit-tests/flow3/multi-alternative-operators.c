#pragma safety enable

/* binary operators pair alternatives by branch: a {1,3}, b {2,4} gives a + b {3, 7}, a * b {2, 12}, a - b {-1} */

void arithmetic(int c)
{
    int a, b;
    if (c) { a = 1; b = 2; }
    else   { a = 3; b = 4; }

    /* a + b in {3, 7} -- correlation excludes 5 */
    compile_assert(a + b == 3 || a + b == 7);
    compile_assert(a + b != 5);
    compile_assert(a + b >= 3);
    compile_assert(a + b <= 7);

    /* a * b in {2, 12} */
    compile_assert(a * b == 2 || a * b == 12);
    compile_assert(a * b != 0);

    /* a - b is exactly {-1} (both branches give -1) */
    compile_assert(a - b == -1);

    /* not provable: a + b == 3 (7 also possible), a * b == 2 (12 also possible) */
}

/* a is {2, 5}, b is {3}: a % b collapses to exactly {2} */
void collapses_to_one(int c)
{
    int a, b;
    if (c) { a = 2; b = 3; }
    else   { a = 5; b = 3; }

    compile_assert(a % b == 2);
}

/* Three-way join feeding a comparison: x is {10, 20, 30}. */
void three_way(int k)
{
    int x;
    if (k == 0)      x = 10;
    else if (k == 1) x = 20;
    else             x = 30;

    compile_assert(x == 10 || x == 20 || x == 30);
    compile_assert(x >= 10);
    compile_assert(x <= 30);
    compile_assert(x != 0);
}
