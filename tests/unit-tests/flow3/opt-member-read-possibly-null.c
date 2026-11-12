#pragma safety enable

/* reading an _Nullable member yields possibly-null, so `if (list->head != NULL) else` keeps a reachable else and an unguarded dereference warns */

#define NULL ((void*)0)

struct token { struct token* next; struct token* prev; };
struct list { struct token* head; struct token* tail; };

/* The else (head became NULL) is reachable. */
void pop_front(struct list* list)
{
    if (list->head == NULL)
        return;

    struct token* old = list->head;
    list->head = old->next;              /* _Nullable member -> possibly null */

    if (list->head != NULL)
    {
        list->head->prev = NULL;
    }
    else
    {
        list->tail = NULL;               /* reachable */
    }
}

/* A guarded _Nullable member read narrows and is safe to use. */
void guarded(struct token* t)
{
    if (t->next != NULL)
        t->next->prev = NULL;            /* ok: guarded */
}
