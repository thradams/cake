#pragma safety enable

/* copying a moved owner into the _View `tail` is not a second transfer, must not warn "is moved" */

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
