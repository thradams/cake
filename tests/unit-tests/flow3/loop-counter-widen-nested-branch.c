#pragma safety enable

/* a counter incremented on both sides of an unrelated if is still widened, and the suppressed first pass's && result does not survive into the reporting pass (tokenizer.c embed_tokenizer) */

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
