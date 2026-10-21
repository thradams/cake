#pragma safety enable

void free(void* _Owner _Opt p);

struct item {
    int i;
};

struct node {
    struct item* _Opt _Owner item;
};

struct list {
    struct node* _Owner _Opt head;
};

void clear_data(const struct list* list)
{
    if (list->head)
    {
        struct item* _Owner _Opt item = list->head->item; //lint 120
        free(item);
    }
}
