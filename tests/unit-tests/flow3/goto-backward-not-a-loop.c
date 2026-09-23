#pragma safety enable

/* known gap: a backward goto is not analysed as a loop, a is seen as exactly 1; if this assertion starts failing, goto-as-loop was implemented */

void f(int n)
{
    int a = 0;
L:
    a = a + 1;
    if (n > 0)
    {
        n = n - 1;
        goto L;
    }
    /* expected today: a == 1 only (documented gap) */
    compile_assert(a == 1);
}
