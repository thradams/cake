#pragma safety enable

/*
   A variable incremented inside a loop must not keep its pre-loop constant
   value afterwards.

   `n` starts at 0 and is incremented in the loop body, so after the loop it
   is "0 or more", not "exactly 0". Treating it as still EQUAL 0 made every
   later test on it decide statically: `if (n > 8)` looked unreachable, and
   `n - 1` folded to -1, so a perfectly ordinary reverse walk reported both
   "unreachable code" and "array index is negative".

   The unreachable-code half is the serious one: the analyser silently stops
   checking the body of a loop it believes cannot run.

   Regression for flow3.c's own chain-walking helpers
   (flow3_map_debug_print, the path printer, the branch-note dedup), which
   all count into a fixed buffer and then walk it backwards.
*/

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
