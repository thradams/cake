#pragma safety enable

/*
   Regression test for is_dead maps.

   When a condition folds to a known constant, one arm becomes
   unreachable (e.g. the false arm of `if (x == 3)` when x == 3 is
   known). Before is_dead existed, "unreachable" was inferred from an
   arm having no map entries at all -- but writes made inside a dead
   arm's (never executed, but still visited for constant folding)
   body could populate it with entries anyway, tricking the merge
   into treating it as a live, reachable arm.
*/

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
