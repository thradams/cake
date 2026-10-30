#pragma safety enable

/* 85 is off by default and the suite passes -wd85; the folded condition is most visible through it */
#pragma cake diagnostic warning 85

/* the loop exit also gets a "left after two or more iterations" arm, so `if (b != 0)` after the loop is not always false (tokenizer.c #line) */

struct token { int type; struct token* next; };

void with_for(struct token* _Nullable head)
{
    struct token* a = 0;
    struct token* b = 0;

    for (struct token* p = head; p; p = p->next)
    {
        if (a == 0)
        {
            a = p;
        }
        else if (b == 0)
        {
            b = p;
        }
    }

    if (b != 0)
    {
        int reached = 1; /* was reported unreachable when the exit state knew only one iteration */
        (void)reached;
    }
}

void with_while(struct token* _Nullable head)
{
    struct token* a = 0;
    struct token* b = 0;
    struct token* p = head;

    while (p)
    {
        if (a == 0)
        {
            a = p;
        }
        else if (b == 0)
        {
            b = p;
        }
        p = p->next;
    }

    if (b != 0)
    {
        int reached = 1;
        (void)reached;
    }
}
