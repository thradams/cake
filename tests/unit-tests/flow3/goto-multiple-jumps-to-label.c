#pragma safety enable

/* every arrival at a label (each goto and fall-through) unions into its map: a == 1, 2 and 3 are all visible */

void f(int x, int y)
{
    int a = 0;
    if (x == 1)
    {
        a = 1;
        goto L;
    }
    if (y == 2)
    {
        a = 2;
        goto L;
    }
    a = 3;
L:
    /* expected: a == 1, a == 2 (both via goto), a == 3 (via fall-through) */
    compile_assert(a == 1 || a == 2 || a == 3);
    compile_assert(a != 0);
}
