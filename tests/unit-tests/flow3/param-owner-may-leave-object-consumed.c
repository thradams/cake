#pragma safety enable

/*
   Which parameter kinds may leave the caller's object consumed.

   A plain pointer parameter is a BORROW: the caller keeps using the pointee,
   so an _Owner member the callee releases must be put back before every exit
   (warning 72).

   An _Owner parameter is NOT a borrow -- ownership transferred in, the caller
   cannot use the pointee afterwards, and it has no expectation about the
   pointee's members. So the callee is free to consume them and warning 72 must
   NOT fire. What the callee still owes is the release of the pointee and of
   the pointer itself; that obligation is a separate leak check (warning 29),
   which the third function below shows still firing.

   The warning text used to read "only a _Dtor parameter may leave the caller's
   object consumed", which contradicted this: _Owner may too.
*/

struct item { int v; };
struct list { struct item* _Owner _Opt head; };

void free_item(struct item* _Owner _Opt p);
void free_list(struct list* _Owner _Opt p);

/* _Owner parameter: consuming the pointee's member is allowed, and the
   pointee and pointer are both released. No diagnostic. */
void owner_param_consumes_member_and_releases(struct list* _Owner p)
{
    free_item(p->head); /* released, never reassigned -- allowed here */
    free_list(p);
}

/* _Owner parameter that does NOT release: the consuming is still fine, but
   the leak obligation remains. */
void owner_param_leaks(struct list* _Owner p)
{
    free_item(p->head);
} //lint 29 p is not released

/* Plain pointer parameter: the caller still owns the pointee, so leaving a
   member consumed is an error. */
void plain_param_consumes_member(struct list* l)
{
    free_item(l->head);
} //lint 72 l->head left consumed

/* Plain pointer parameter, consumed on one path only -- still an error, the
   caller can reach the exit with l->head dangling. */
void plain_param_consumes_on_one_path(struct list* l, int flag)
{
    if (flag)
    {
        free_item(l->head);
    }
} //lint 72 l->head left consumed on the flag path
