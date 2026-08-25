#pragma safety enable

/* 85 is off by default and the suite passes -wd85; the folded condition is
   most directly visible through it. */
#pragma cake diagnostic warning 85

/*
   The state after a loop must account for the loop running TWO or more times,
   not just once.

       for (p = head; p; p = p->next)
       {
           if (a == 0) { a = p; }
           else if (b == 0) { b = p; }   <- only ever runs from iteration 2 on
       }
       if (b != 0) { ... }               <- NOT always false

   The loop's exit arms used to be "condition false before any iteration",
   "condition false after one iteration" and the break-join -- all computed
   before the second pass's body runs. Pass 1 always takes the `a == 0` arm,
   so `b` was never assigned in any state that reached the exit, and the test
   after the loop folded to false with its body reported unreachable.

   flow_visit_while_statement and flow_visit_for_statement now take a third
   look at the condition after the second pass's body, giving the exit a
   "left after two or more iterations" arm.

   Reduced from src/tokenizer.c control_line's `#line` handling, which walks
   the directive's tokens picking up the line number first and the filename
   second, then tests `p_filename != NULL`.
*/

struct token { int type; struct token* _Opt next; };

void with_for(struct token* _Opt head)
{
    struct token* _Opt a = 0;
    struct token* _Opt b = 0;

    for (struct token* _Opt p = head; p; p = p->next)
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
        int reached = 1; /* reported "unreachable code" while the exit state
                            only knew about one iteration */
        (void)reached;
    }
}

void with_while(struct token* _Opt head)
{
    struct token* _Opt a = 0;
    struct token* _Opt b = 0;
    struct token* _Opt p = head;

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
