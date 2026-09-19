#pragma safety enable
#define NULL ((void*)0)

/* `while (head) { next = head->next; node_delete(head); head = next; }` leaves head == NULL, no false "owner not moved": an empty entry narrows like ANY on the exit arm */

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

/* contrast, must warn: with an `if` the body runs at most once and the last node leaks */
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
