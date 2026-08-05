#pragma safety enable

/*
   Regression test: the classic linked-list traversal idiom

       for (struct node* p = get(); p; p = p->next) { ... }

   The `p` in the condition narrows p to non-null for the rest of that
   iteration, so both the increment's `p->next` and any `p->` access in
   the body must be considered safe -- no null-dereference warning.

   Contrast with traverse_unguarded below: same shape, but the condition
   checks something unrelated to p (`n > 0`), so p is never narrowed and
   `p->next` in the increment is a possible null dereference.
*/

struct node
{
    struct node* _Opt next;
    int value;
};

struct node* _Opt get();

void traverse(void)
{
    for (struct node* _Opt p = get(); p; p = p->next)
    {
        /* p narrowed to non-null by the for-condition -- safe to call p-> */
        int v = p->value;
        (void)v;
    }
}

void traverse_unguarded(int n)
{
    for (struct node* _Opt p = get(); n > 0; p = p->next) // -> operator applied to a null pointer
    {
        n--;
    }
} //lint 33 -> operator applied to a possible null pointer 'p'
