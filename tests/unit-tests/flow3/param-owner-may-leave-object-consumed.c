#pragma safety enable

/* a plain pointer parameter is a borrow (72 if a member is left consumed); an _Owner parameter may consume members but still owes the release (29) */

struct item { int v; };
struct list { struct item* _Owner _Opt head; };

void free_item(struct item* _Owner _Opt p);
void free_list(struct list* _Owner _Opt p);

/* _Owner parameter: member consumed, pointee and pointer released, no diagnostic */
void owner_param_consumes_member_and_releases(struct list* _Owner p)
{
    free_item(p->head); /* released, never reassigned -- allowed here */
    free_list(p);
}

/* _Owner parameter that does not release: consuming is fine, the leak remains */
void owner_param_leaks(struct list* _Owner p)
{
    free_item(p->head);
} //lint 29 p is not released

/* plain pointer parameter: leaving a member consumed is an error */
void plain_param_consumes_member(struct list* l)
{
    free_item(l->head);
} //lint 72 l->head left consumed

/* plain pointer parameter, consumed on one path only: still an error */
void plain_param_consumes_on_one_path(struct list* l, int flag)
{
    if (flag)
    {
        free_item(l->head);
    }
} //lint 72 l->head left consumed on the flag path
