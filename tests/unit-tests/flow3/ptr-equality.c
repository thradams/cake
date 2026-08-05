#pragma safety enable

/*
   Two unrelated pointers may still compare equal at run time.

   Known issue: flow3_alternatives_is_single_constant treats a concrete
   (synthetic arena) pointer value as a foldable constant, so `p == q`
   with two distinct pointees folds to always-false and the true branch
   is analysed as dead code.
*/

void ptr_eq(int* _Opt p, int* _Opt q)
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

void ptr_eq_null(int* _Opt p)
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
