#pragma safety enable

/*
   Regression test (was a known gap, now FIXED): a possible null
   dereference must still be reported when the exact same code sits
   inside a loop, and repeating an identical possibly-null access must
   warn every time -- one occurrence "consuming" the null alternative
   must not silence the next identical one.

   This used to be missed: loops warm up state with a first pass that
   runs with diagnostics suppressed (see the two-pass structure in
   flow3_visit_while_statement / flow3_visit_do_while_statement /
   flow3_visit_for_statement), and a one-shot "could still be null"
   alternative could be consumed by that suppressed pass before the
   real (diagnostics-on) pass ever saw it -- so a null dereference
   inside a loop body/increment went completely unreported. Both
   functions below now correctly warn on every occurrence.
*/

struct node
{
    struct node* _Opt next;
};

struct node* _Opt get();

void seq_first_only(void)
{
    struct node* _Opt p = get();
    p = p->next; //lint 33 -> operator applied to a null pointer
    p = p->next; //lint 33 -> operator applied to a null pointer
}

void loop_swallows_it_entirely(int n)
{
    /* Same possible-null access as seq_first_only, now correctly
       flagged even though it sits in the loop's increment. */
    for (struct node* _Opt p = get(); n > 0; p = p->next) // -> operator applied to a null pointer
    {
        n--;
    }
} //lint 33 -> operator applied to a possible null pointer 'p'
