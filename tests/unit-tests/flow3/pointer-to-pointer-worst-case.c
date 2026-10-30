#pragma safety enable

/* T** pp: *pp was written by someone else, so like a member it may be null unless _Nonnull (flow.md 2.2) */

struct token { int type; };

void next_token(struct token** pp);
void next_token_nonnull(struct token* _Nonnull* pp);

void reads_may_be_null(struct token** pp)
{
    int i = (*pp)->type; //lint 33
}

void reads_nonnull(struct token* _Nonnull* pp)
{
    int i = (*pp)->type; /* ok */
}

void reads_after_test(struct token** pp)
{
    if (*pp)
    {
        int i = (*pp)->type; /* ok */
    }
}

void writes_null_ok(struct token** pp)
{
    *pp = 0; /* ok: *pp may be null */
}

void writes_null_nonnull(struct token* _Nonnull* pp)
{
    *pp = 0; //lint 35
}

void caller_reads_after(struct token* p)
{
    next_token(&p);
    int i = p->type; //lint 33
}

void caller_reads_after_nonnull(struct token* p)
{
    next_token_nonnull(&p);
    int i = p->type; /* ok: the callee cannot leave it null */
}
