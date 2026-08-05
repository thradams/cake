#pragma safety enable

/*
   Checkpoint: multi-alternative values with CORRELATED joins.

   A variable can hold several values after a control-flow join, each tagged
   with the branch (a stable branch-decision "origin") it came from. Values
   combine only across compatible origins -- no branch id appears on both
   sides with opposite outcomes -- so a join stays correlated all the way
   through arithmetic, comparisons, and &&/|| . A constant (or constexpr)
   carries no branch decisions, so it correlates with every path.

   Every compile_assert here holds on EVERY path, so this file is clean
   (0 errors / 0 warnings). The cases that should fail are documented in
   comments only.
*/

/* ---- arithmetic keeps the branch pairing: a+b in {3,7}, not {3,5,7} ---- */
void arithmetic(int c)
{
    int a, b;
    if (c) { a = 1; b = 2; } else { a = 3; b = 4; }

    compile_assert(a + b == 3 || a + b == 7);   /* 5 is excluded by correlation */
    compile_assert(a + b != 5);
    compile_assert(a * b == 2 || a * b == 12);
    compile_assert(a - b == -1);                /* both branches give -1 */
    /* NOT provable: a + b == 3   (7 also possible) */
}

/* ---- relational folds over all alternatives ---- */
void relational(int c)
{
    int a, b;
    if (c) { a = 1; b = 2; } else { a = 3; b = 4; }

    compile_assert(a + b >= 3);
    compile_assert(a + b <= 7);
    compile_assert(a < b);                       /* 1<2 and 3<4 (correlated) */
}

/* ---- equality correlates two variables ---- */
void equality(int c)
{
    int a, b;
    if (c) { a = 1; b = 1; } else { a = 2; b = 2; }

    compile_assert(a == b);                      /* 1==1 and 2==2 */

    int x, y;
    if (c) { x = 1; y = 2; } else { x = 3; y = 4; }
    compile_assert(x != y);                      /* 1!=2 and 3!=4 */
}

/* ---- per-path || and && : true on every path ---- */
void logical(int c)
{
    int a, b;
    if (c) { a = 1; b = 2; } else { a = 3; b = 4; }

    compile_assert(a + b == 3 || a + b == 7);
    compile_assert(a + b < 5 || a + b > 6);
    compile_assert(a + b != 5 && a + b != 100);
    /* NOT provable: a+b==4 || a+b==7  (the a+b==3 path satisfies neither) */
}

/* ---- constexpr behaves like a literal constant (no branch decisions) ---- */
void with_constexpr(int c)
{
    constexpr int K = 5;
    int a;
    if (c) a = 5; else a = 10;

    compile_assert(a + K == 10 || a + K == 15);  /* a+K in {10,15} */
    compile_assert(a == K || a == 10);
    /* NOT provable: a == K   (a can be 10) */
}

/* ---- three-way join ---- */
void three_way(int k)
{
    int x;
    if (k == 0)      x = 10;
    else if (k == 1) x = 20;
    else             x = 30;

    compile_assert(x == 10 || x == 20 || x == 30);
    compile_assert(x >= 10 && x <= 30);
}
