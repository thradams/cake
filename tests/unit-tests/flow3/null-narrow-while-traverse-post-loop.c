#pragma safety enable

/* `while (p) { p = p->next; }` narrows p inside the loop; after it p is null again and a dereference must warn */

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

/* contrast: p-> inside the body is safe */
void traverse_inside_only(struct node* _Opt p)
{
    while (p)
    {
        int v = p->v; /* ok: narrowed non-null */
        p = p->next;   /* ok: same */
    }
}
