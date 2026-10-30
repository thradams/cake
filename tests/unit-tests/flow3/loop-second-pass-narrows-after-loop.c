#pragma safety enable

/* FIXED: `found` assigned at most once inside the loop is not folded non-null after it; widening pointer identity was tried and reverted (breaks the free-and-advance idiom) */

struct node
{
    int value;
    struct node* next;
};

void after_loop_wrongly_non_null(struct node* _Nullable head)
{
    struct node* found = 0;

    for (struct node* p = head; p; p = p->next)
    {
        if (found == 0)
        {
            found = p;
        }
    }

    if (found == 0) /* reachable: head could be NULL, or every node already matched */
    {
        int reached = 1; /* reported "unreachable code" if the test folds */
        (void)reached;
    }
}
