#pragma safety enable

/* FIXED: a possible null dereference inside a loop is reported, and every repeated identical access warns, the suppressed warm-up pass does not consume the null alternative */

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
    /* same access as seq_first_only, flagged even in the loop's increment */
    for (struct node* _Opt p = get(); n > 0; p = p->next) // -> operator applied to a null pointer
    {
        n--;
    }
} //lint 33 -> operator applied to a possible null pointer 'p'
