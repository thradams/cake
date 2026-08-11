#pragma safety enable

/*
   Linked-list append: `tail` is a _View (non-owner) pointer to the last node.
   After the node is moved into the list (`head = pnew` or `tail->next = pnew`),
   copying pnew into the view `tail` is NOT a second ownership transfer -- the
   object is still alive, owned by the list -- so it must not warn "is moved".
   (Assigning a moved owner to another OWNER would still warn: use-after-move.)
*/

#define NULL ((void*)0)

struct item {
  int i;
  struct item * _Owner _Opt next;
  struct item * _Opt previous;
};

struct list
{
    struct item* _Owner _Opt head;
    struct item* _Opt tail;
};
void list_push(struct list* list, struct item* _Owner pnew)
{
    if (list->head == NULL)
    {
        list->head = pnew;
        list->tail = pnew; // ok: tail is a view (not a second ownership transfer)
    }
    else
    {
        _Assert(list->tail != NULL);
        _Assert(list->tail->next == NULL); // -> operator applied to a null pointer
        pnew->previous = list->tail;
        list->tail->next = pnew; // move pnew into the list
        list->tail = pnew; // ok: tail is a view (not a second ownership transfer)
    }
    
}
