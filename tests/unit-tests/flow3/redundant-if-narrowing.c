#pragma safety enable

/* `if (p) { if (p) { ... } }`: p stays non-null inside the redundant inner if */

void inner_if_sees_nonnull(int* _Opt p)
{
    if (p)
    {
        if (p)
        {
            *p = 1; /* ok: p is non-null here, proven twice over */
        }
    }
}

/* KNOWN GAP: after an empty redundant `if (p) {}` the non-null narrowing is lost (the false arm synthesizes `== 0` for the while-exit case); marking that arm unreachable was tried and reverted */
void after_redundant_if_loses_nonnull(int* _Opt p)
{
    if (p)
    {
        if (p)
        {
        }
        *p = 1; //lint 33 known false positive: possible null pointer dereference
    }
}
