#pragma safety enable

void free(void* _Owner _Nullable p);

struct node {
    int i;
};

struct list {
    struct node* _Owner head;
};

void clear_data(const struct list* list)
{
    struct node* _Owner head = list->head; //lint 99
    free(head);
} //lint 72
