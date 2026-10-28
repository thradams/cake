#pragma safety enable

/* moving a member through one parameter moves it only on the paths where that parameter points to the object (token.c token_list_remove_get) */

struct T { struct T* _Owner _Opt next; };
void keep(struct T* _Owner _Opt p);

/*
    entry flow:

    first = new();
    if (last == first) last = first;
    else               last = new();
*/

/* Must NOT warn: when first != last only last->next moves; when first == last it is reassigned. */
void move_and_reassign(struct T* first, struct T* last)
{
    struct T* _Owner _Opt after = last->next;
    last->next = 0;
    keep(after);
    (void)first;
}

/* must warn: last->next is moved and never reassigned */
void move_only(struct T* first, struct T* last)
{
    struct T* _Owner _Opt after = last->next;
    keep(after);
    (void)first;
} //lint 72 72
