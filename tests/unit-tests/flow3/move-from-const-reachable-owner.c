#pragma safety enable

void free(void* _Owner _Nullable p);

struct item {
    int i;
};

struct node {
    struct item* _Owner item;
};

struct list {
    struct node* _Owner head;
};

void clear_data(const struct list* list)
{
    if (list->head)
    {
        struct item* _Owner item = list->head->item; //lint 120
        free(item);
    }
}
