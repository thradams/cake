#pragma safety enable

/* compile_assert(X || Y) is evaluated per path: after the if/else a+b is {3, 7} and the || combines the per-path booleans */

void must_pass(int c)
{
    int a, b;
    if (c) { a = 1; b = 2; }
    else   { a = 3; b = 4; }

    /* Every value of a+b (3 or 7) satisfies one side. */
    compile_assert(a + b == 3 || a + b == 7);
    compile_assert(a + b < 5 || a + b > 6);      /* 3<5, 7>6 */

    /* Three-way join, three-way OR. */
    int x;
    if (c) x = 10; else x = 20;
    compile_assert(x == 10 || x == 20);
}

/* these fail if enabled: `a + b == 4 || a + b == 7` (3 satisfies neither), `a + b < 3 || a + b > 7` */

#pragma safety enable
void f(int c){
    int a, b;
    if (c) { a = 1; b = 2; } else { a = 3; b = 4; }

    /* MUST pass (every path satisfies one side) */
    compile_assert(a + b == 3 || a + b == 7);
    compile_assert(a + b != 5 && a + b != 100);

// MUST fail (a+b==3 path satisfies neither) -- enable one to check:
       compile_assert(a + b == 4 || a + b == 7); //lint 67 a+b==3 path satisfies neither side
       compile_assert(a + b == 3 || a + b == 5); //lint 67 mixed-left
       compile_assert(a + b == 3 && a + b == 7); //lint 67 67 mixed-left AND (fails on both arms)
    
}
