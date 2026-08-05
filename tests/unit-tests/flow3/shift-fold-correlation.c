#pragma safety enable

/*
   Shift operators (<< >>) fold like the other binary arithmetic: each operand
   alternative is combined on its own path, keeping the branch origin, so a
   join survives the shift.

   Use compile_assert (flow-checked), not static_assert (C11 compile-time).
*/

/* A correlated join survives a shift: n is {1@then, 2@else}, so 1<<n is
   {2, 4} -- never 1<<something-in-between. */
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

/*
   KNOWN LIMITATION -- inline re-fold inside || loses correlation.

   A foldable expression written twice inside the same || is recomputed
   independently, so the two sides are not tied to the same path:

       void limitation(int n) {
           if (n == 1 || n == 2) {
               // FAILS today, though every path satisfies one side:
               compile_assert((1 << n) == 2 || (1 << n) == 4);
           }
       }

   Workaround: bind the value once, then compare the single variable, which
   keeps the correlation (this is exactly what by_variable() above does):

       int v = 1 << n;
       compile_assert(v == 2 || v == 4);
*/
