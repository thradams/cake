#pragma safety enable

/* copying a moved owner into the _View `tail` is not a second transfer, must not warn "is moved" */

#define NULL ((void*)0)

struct item {
  int i;
  struct item * _Owner next;
  struct item * previous;
};

struct list
{
    struct item* _Owner head;
    struct item* tail;
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
        pnew->previous = list->tail;
        list->tail->next = pnew; //lint 33 26 tail may be null, tail->next may own
        list->tail = pnew; // ok: tail is a view (not a second ownership transfer)
    }
    
}
