struct item {
    int i;
    struct item * next;
    struct item * previous;
};

struct list {
    struct item * head;
    struct item * tail;
};


void list_push(struct list * list, struct item * pnew)
{
    if (list->head == 0U)
    {
        list->head = pnew;
        list->tail = pnew;
    }
    else
    {
        ;
        ;
        pnew->previous = list->tail;
        list->tail->next = pnew;
        list->tail = pnew;
    }
}

