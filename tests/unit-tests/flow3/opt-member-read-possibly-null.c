#pragma safety enable

/*
   Reading an _Opt pointer member yields a POSSIBLY-NULL value.

   flow3 used to leave an _Opt member read with no value at all (empty). An
   empty pointer was then treated as definitely non-null by a later
   `x != NULL` test, wrongly killing the else branch. e.g.:

       list->head = old_head->next;     // next is _Opt -> possibly null
       if (list->head != NULL) { ... }
       else { list->tail = NULL; }      // was flagged UNREACHABLE

   Now an unseeded pointer member is seeded from its declared nullability:
   non-_Opt -> non-null; _Opt -> possibly-null. So the else stays reachable,
   and dereferencing an _Opt member without a guard correctly warns.
*/

#define NULL ((void*)0)

struct token { struct token* _Opt next; struct token* _Opt prev; };
struct list { struct token* _Opt head; struct token* _Opt tail; };

/* The else (head became NULL) is reachable. */
void pop_front(struct list* list)
{
    if (list->head == NULL)
        return;

    struct token* _Opt old = list->head;
    list->head = old->next;              /* _Opt member -> possibly null */

    if (list->head != NULL)
    {
        list->head->prev = NULL;
    }
    else
    {
        list->tail = NULL;               /* reachable */
    }
}

/* A guarded _Opt member read narrows and is safe to use. */
void guarded(struct token* t)
{
    if (t->next != NULL)
        t->next->prev = NULL;            /* ok: guarded */
}
