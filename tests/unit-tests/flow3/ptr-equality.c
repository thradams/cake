#pragma safety enable

/* known issue: `p == q` with two distinct synthetic pointees folds to always-false and the true branch is analysed as dead */

void ptr_eq(int* _Nullable p, int* _Nullable q)
{
    if (p == q)
    {
        /* This branch is reachable and must be analysed. */
        if (p)
        {
            compile_assert(p != 0);
        }
    }
}

void ptr_eq_null(int* _Nullable p)
{
    /* Comparison against the genuine constant 0 must still narrow. */
    if (p == 0)
    {
        compile_assert(p == 0);
    }
    else
    {
        compile_assert(p != 0);
    }
}
