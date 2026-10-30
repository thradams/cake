#pragma safety enable

/* moving an owner reachable from a parameter writes it: the parameter is not suggested const (82), since const would forbid the move (99/120) */

void free(void* _Owner _Nullable p);

struct item { int i; };
struct node { struct item* _Owner item; };
struct list { struct item* _Owner head; };
struct list2 { struct node* _Owner head; };

void release_head(struct list* list)
{
    free(list->head);
    list->head = 0;
}

void release_item(struct list2* list)
{
    if (list->head)
    {
        free(list->head->item);
        list->head->item = 0;
    }
}