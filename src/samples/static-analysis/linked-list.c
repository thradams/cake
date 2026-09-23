#pragma safety enable

#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct item {
    char* _Owner title;
    struct item* _Owner _Opt next;
};

struct list {
    struct item* _Owner _Opt head;
    struct item* _Opt tail;
};

void list_insert_after(struct list* list,
    struct item* item,
    struct item* _Owner p_new_item)
{
    if (item->next == NULL) {
        list->tail = p_new_item;
    }
    else {
        assert(p_new_item->next == NULL);
        p_new_item->next = item->next;
    }

    item->next = p_new_item;
}

void list_push_back(struct list* list,
    struct item* _Owner p_item)
{
    if (list->head == NULL) {
        list->head = p_item;
    }
    else {
        assert(list->tail);
        assert(list->tail->next == nullptr);
        list->tail->next = p_item;
    }
    list->tail = p_item;
}

void list_push_front(struct list* list,
    struct item* _Owner p_item)
{
    if (list->head == NULL) {
        list->tail = p_item;
    }
    else {
        assert(p_item->next == NULL);
        p_item->next = list->head;
    }
    list->head = p_item;
}

void list_destroy(_Dtor struct list* list)
{
    struct item* _Owner _Opt it = list->head;
    while (it != NULL) {
        struct item* _Owner _Opt next = it->next;
        free(it->title);
        free(it);
        it = next;
    }
}

int main(int argc, char* argv[])
{
    struct list list = { 0 };
    struct item* _Owner _Opt p_item = calloc(1, sizeof(struct item));
    if (p_item)
    {
        char * _Owner _Opt title = strdup("title");
        if (title)
        {
            p_item->title = title;
            list_push_front(&list, p_item);
        }
        else
        {
            free(p_item);
        }
    }
    list_destroy(&list);
}
