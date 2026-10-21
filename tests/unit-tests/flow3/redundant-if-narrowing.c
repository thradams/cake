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

/* after an empty redundant `if (p) {}` the non-null narrowing is kept: the inner false arm is unreachable */
void after_redundant_if_loses_nonnull(int* _Opt p)
{
    if (p)
    {
        if (p)
        {
        }
        *p = 1;
    }
}
