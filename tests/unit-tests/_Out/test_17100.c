#pragma safety enable



#define NULL ((void*)0)

struct item {
  int i;
  struct item * /*_Owner*/ /*_Opt*/ next;
  struct item * /*_Opt*/ previous;
};

struct list
{
    struct item* /*_Owner*/ /*_Opt*/ head;
    struct item* /*_Opt*/ tail;
};
void list_push(struct list* list, struct item* /*_Owner*/ pnew)
{
    if (list->head == NULL)
    {
        list->head = pnew;
        list->tail = pnew;
    }
    else
    {
        assert(list->tail != NULL);
        assert(list->tail->next == NULL);
        pnew->previous = list->tail;
        list->tail->next = pnew;
        list->tail = pnew;
    }
    
}