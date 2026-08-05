#pragma safety enable


struct item {
    struct item* _Owner _Opt next;
};
void item_delete(struct item* _Owner p);

struct list {
    struct item* _Owner _Opt head;
    struct item* _Opt tail;
};

void list_destroy(_Dtor struct list*  list)
{
    struct item* _Owner _Opt p = list->head; // ok: loop frees every node; p is NULL on exit
    while (p)
    {
        struct item* _Owner _Opt next = p->next;
        p->next = 0;
        item_delete(p);
        p = next;
    }
}

int main()
{
    struct list list = { 0 };
    list_destroy(&list);
}
