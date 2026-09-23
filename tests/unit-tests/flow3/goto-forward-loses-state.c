#pragma safety enable

/* a forward goto snapshots its state into the label's map, so both a == 1 (goto) and a == 2 (fall-through) survive */

void f(int x)
{
    int a = 1;
    if (x)
        goto L;
    a = 2;
L:
    /* expected: a == 1 (arrived via goto), a == 2 (arrived via fall-through) */
    compile_assert(a == 1 || a == 2);
    compile_assert(a != 0);
}
