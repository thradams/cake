#pragma safety enable

/*
   A counter incremented once per iteration must not keep the first
   iteration's concrete value in the pass that reports.

   Two things used to lose it, and both showed up as a folded condition
   (`count > 0 && count % 25 == 0` reported always-false, its body reported
   unreachable) in tokenizer.c's embed_tokenizer:

   1. The && result seeded by the loop's SUPPRESSED first pass stayed in the
      map, so the reporting pass read a value it no longer believed.
   2. Incrementing the counter on both sides of an unrelated `if` leaves two
      alternatives that both say the same number, and the loop widening
      only recognized a single alternative -- so the counter was never
      widened to ANY.
*/

int next(void);

void counter_in_while(void)
{
    int count = 0;
    while (next())
    {
        if (count > 0 && count % 25 == 0)
        {
            int reached = 1; /* reachable: count is not stuck at its first value */
            (void)reached;
        }
        count++;
    }
}

void counter_under_nested_branch(void)
{
    int count = 0;
    _Bool b_first = 1;
    while (next())
    {
        if (b_first)
        {
            b_first = 0;
        }
        else
        {
            if (count > 0 && count % 25 == 0)
            {
                int reached = 1;
                (void)reached;
            }
        }
        count++;
    }
}

void counter_in_for(void)
{
    int count = 0;
    for (; next(); )
    {
        if (count > 0 && count % 25 == 0)
        {
            int reached = 1;
            (void)reached;
        }
        count++;
    }
}

void counter_in_do_while(void)
{
    int count = 0;
    do
    {
        if (count > 0 && count % 25 == 0)
        {
            int reached = 1;
            (void)reached;
        }
        count++;
    } while (next());
}
