#pragma safety enable

/* a for-loop whose body never falls through is still visited with diagnostics on, starting from the condition's true branch (as while does), so the body sees the condition's narrowing */

struct X { int i; };

void narrowing_reaches_body(struct X* _Opt p)
{
    for (; p != 0;)
    {
        p->i = 1; /* p is not null here */
        return;
    }
}

void narrowing_reaches_body_while(struct X* _Opt p)
{
    while (p != 0)
    {
        p->i = 1;
        return;
    }
}
