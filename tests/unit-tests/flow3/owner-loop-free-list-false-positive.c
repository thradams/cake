#pragma safety enable
#define NULL ((void*)0)

/*
   Regression test: the classic "free an owned linked list" loop must
   NOT emit a false

       warning 31: owner object (head) not moved

   Every node -- including the final value of `head` -- is released, and
   `head` is provably NULL once the loop exits, so nothing leaks.

       while (head)
       {
           struct node* _Owner _Opt next = head->next;
           head->next = NULL;
           node_delete(head);   // releases this node
           head = next;         // advance; head becomes NULL at the end
       }
       // head == NULL here on every path -- no leak

   Root cause of the former false positive (now fixed in flow3.c):
   after the body's `head = next;` the cursor's tracked entry collapses
   to EMPTY. On the loop's second (real-diagnostics) pass, narrowing
   `while (head)` found that empty entry and produced no `head == 0`
   fact for the false/exit arm. The loop-exit merge
   (flow3_map_merge_arms) then fell back to `head`'s stale pre-loop value
   (a live, non-null owner) for the exit arm, and
   flow3_check_object_at_exit reported the bogus "owner not moved".
   flow3_narrow_map_into now treats an empty (unknown) entry the same as
   FLOW3_RELATION_ANY -- true arm => nonzero, false arm => exactly zero
   -- so the exit arm correctly records head == NULL.

   The same fix also makes post-loop `p == 0` provable (see safety-045)
   and short-circuit narrowing like `if (pX && pX->pi)` work (safety-129).
*/

struct node
{
    struct node* _Owner _Opt next;
};

void node_delete(struct node* _Owner _Opt p);

/* Must NOT warn: the loop frees the whole list and leaves head == NULL. */
void free_list(struct node* _Owner _Opt head)
{
    while (head)
    {
        struct node* _Owner _Opt next = head->next;
        head->next = NULL; /* detach so head is the sole owner of *head */
        node_delete(head); /* release this node */
        head = next;       /* advance */
    }
}

/* Contrast (true positive, MUST still warn): with an `if` the body runs
   at most once, so the last value moved into head is never released
   before head leaves scope -- a genuine leak. */
void leaks_when_not_a_loop(struct node* _Owner _Opt head)
{
    if (head)
    {
        struct node* _Owner _Opt next = head->next;
        head->next = NULL;
        node_delete(head);
        head = next; // owner object (head) not moved: real leak, must warn
    }
} //lint 29 owner object (head) not moved (opt-mem-nonnull see line 64)
