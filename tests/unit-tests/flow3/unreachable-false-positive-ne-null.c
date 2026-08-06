#pragma safety enable

/*
   False "unreachable code" (warning 68) on the else-branch assignment when
   the condition is written as `p != NULL` (or `p != 0`) inside a loop, for
   a nullable pointer that starts NULL and gets assigned on some iteration.
   Rewriting the exact same condition as plain pointer truthiness (`if (p)`)
   makes the false positive disappear -- so the bug is specifically in how
   the `!=` comparison narrows/merges the loop-carried state, not in the
   unreachable-code check itself.

   Reduced from src/expressions.c's generic-selection parsing loop
   (p_default_generic_association_first_token), around line 548, where
   changing `if (p_default_generic_association_first_token != NULL)` to
   `if (p_default_generic_association_first_token)` made the warning go
   away. User-reported.
*/

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
            p = &cond; //lint 68  TODO FALSE WARNING
        }
    }
}
