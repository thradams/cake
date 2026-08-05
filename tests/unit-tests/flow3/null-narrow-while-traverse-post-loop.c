#pragma safety enable

/*
   Regression test for the classic linked-list traversal idiom using a
   while loop (contrast with null-narrow-for-traverse.c, which covers
   the equivalent for-loop form):

       while (p) { p = p->next; }
       p->v; // p is null here -- must still warn

   Inside the loop, `p` is narrowed to non-null by the while condition,
   so both `p->next` in the body and any other `p->` access there are
   safe. Once the loop exits (whether because p naturally reached NULL
   at the end of the list, or the loop never ran because p started
   NULL), p is possibly null again -- and flow3 must still catch a
   dereference of it after the loop.
*/

struct node
{
    struct node* _Opt next;
    int v;
};

void traverse(struct node* _Opt p)
{
    while (p)
    {
        /* p narrowed to non-null by the while condition -- safe */
        int v = p->v;
        (void)v;
        p = p->next;
    }
    int x = p->v; //lint 33 -> operator applied to a null pointer
}

/* Contrast: accessing p-> INSIDE the loop body is always safe, since
   the while condition already proved p non-null for this iteration. */
void traverse_inside_only(struct node* _Opt p)
{
    while (p)
    {
        int v = p->v; /* ok: narrowed non-null */
        p = p->next;   /* ok: same */
    }
}
