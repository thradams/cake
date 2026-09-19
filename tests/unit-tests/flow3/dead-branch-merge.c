#pragma safety enable

/* a constant-folded arm is dead even if visiting it left entries in its map; the merge must skip it */

void known_true_branch(void)
{
    int x = 3;
    int a = 0;
    if (x == 3)
    {
        a = 1;
    }
    else
    {
        /* unreachable: x == 3 is a compile-time fact */
        a = 2; //lint 68 unreachable code
    }
    compile_assert(a == 1);
    compile_assert(a != 2);
}

void known_false_branch(void)
{
    int x = 3;
    int a = 0;
    if (x == 4)
    {
        /* unreachable */
        a = 1; //lint 68 unreachable code
    }
    else
    {
        a = 2;
    }
    compile_assert(a == 2);
    compile_assert(a != 1);
}
