#pragma safety enable

/*
  After f(p), p->next may point to a new object (f is not _Dtor, it never
  deletes *p, but it may replace p->next). Reading through p->next follows the
  new path: ok. Only an alias saved before the call can be dangling.
*/

struct token
{
    struct token* _Owner next;
    int type;
};

void f(struct token* p);

void new_path(struct token* p)
{
    f(p);
    struct token* q = p->next; /* ok: new path */
    if (q)
        q->type = 1;
}

void old_alias(struct token* p)
{
    struct token* old = p->next;
    f(p);
    if (old)
        old->type = 1; //lint 31 the old object may have been deleted
}

/* f is called only on one path: on that path p->next is new, on the other it is the old (alive) object */
void one_path(struct token* p, int c)
{
    if (c)
        f(p);
    struct token* q = p->next; /* ok */
    if (q)
        q->type = 1;
}

/* do-while: false positive 31 on ptk->next (the same loop as while does not warn) */
void do_while_walk(struct token* first)
{
    struct token* ptk = first;
    do
    {
        if (ptk == 0)
            break;
        if (ptk->type)
            f(ptk);
        ptk = ptk->next; /* ok */
    } while (ptk);
}
