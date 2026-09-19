#pragma safety enable

/* FIXED: `if (p != NULL)` in a loop on a pointer assigned on some iteration must not report the else branch unreachable; the diagnostic pass unions the pre-loop value per key (expressions.c generic-selection loop) */

#define NULL ((void*)0)

void f(int cond)
{
    int* _Opt p = 0;

    while (cond)
    {
        if (p != NULL)
        {
        }
        else
        {
            p = &cond; /* reachable: the first iteration has p == 0 */
        }
    }
}
