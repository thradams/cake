#pragma safety enable

/* `for (p = get(); p; p = p->next)` narrows p for the iteration, the increment and body are safe; traverse_unguarded tests something else and warns */

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
