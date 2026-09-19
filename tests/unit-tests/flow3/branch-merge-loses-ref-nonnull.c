#pragma safety enable

/* FIXED: `->` re-fabricates a void-typed pointee with the pointer's real type, so `p = &obj->a` keeps a REF and the branch merge no longer falls back to p's pre-if NULL */

#define NULL ((void*)0)


void* _Owner _Opt _Clear calloc(int n, int s);
void free(void* _Owner _Opt p);

struct s { int a; int b; };

void use(int* p);

/* A single branch is fine on its own. */
void one_branch_ok(void)
{
    struct s* _Owner _Opt obj = calloc(1, sizeof * obj);
    if (obj == NULL) return;

    int* _Opt p = &obj->a;

    _Assert(p != NULL);
    use(p); /* ok: no warning */

    free(obj);
}

/* merging arms that assign p to different members of the same _Owner object must keep the non-null fact */
void merged_branches_bug(int cond)
{
    struct s* _Owner _Opt obj = calloc(1, sizeof * obj);
    if (obj == NULL) return;

    int* _Opt p = NULL;

    if (cond)
        p = &obj->a;
    else
        p = &obj->b;

    _Assert(p != NULL);

    /* FIXED: no longer warns, obj->a and obj->b resolve to real member objects */
    use(p);

    free(obj);
}
