#pragma safety enable

/* a counter incremented in a loop is "0 or more" afterwards, not exactly 0: `if (n > 8)` is reachable and `n - 1` is not -1 */

int a[8];

struct node
{
    struct node* _Opt next;
};

int count_then_walk_backwards(struct node* _Opt list)
{
    int n = 0;

    for (struct node* _Opt cur = list; cur; cur = cur->next)
    {
        n++;
    }

    /* Reachable: n may be greater than 8. */
    if (n > 8)
    {
        n = 8;
    }

    int s = 0;

    /* Reachable, and the index is never negative. */
    for (int i = n - 1; i >= 0; i--)
    {
        s += a[i];
    }

    return s;
}

/* The same counter used as a loop bound going forwards. */
int count_then_walk_forwards(struct node* _Opt list)
{
    int n = 0;

    for (struct node* _Opt cur = list; cur; cur = cur->next)
    {
        if (n < 8)
        {
            n++;
        }
    }

    int s = 0;

    for (int i = 0; i < n; i++)
    {
        s += a[i];
    }

    return s;
}

/* A counter incremented in a while loop, then tested. */
int count_in_while(struct node* _Opt list)
{
    int n = 0;
    struct node* _Opt cur = list;

    while (cur != 0)
    {
        n++;
        cur = cur->next;
    }

    if (n == 0)
    {
        return 0;
    }

    return a[n - 1 < 8 ? n - 1 : 7];
}
