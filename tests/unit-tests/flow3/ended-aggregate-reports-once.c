#pragma safety enable

/* a consumed aggregate reports ONCE by object name: ENDED collapses on "any leaf", UNINITIALIZED and MOVED keep "all leaves" (expressions.c: 222 diagnostics for one object) */

#define NULL ((void*)0)

struct inner
{
    int i;
    char* _Owner s;
};

struct wide
{
    struct wide* _Owner a;
    struct wide* _Owner b;
    struct wide* _Owner c;
    const char* name;
    int count;
    struct inner nested;
};

struct wide* _Owner make(void);
struct wide* _Owner tail(struct wide* _Owner p);
void del(struct wide* _Owner _Nullable p);
int maybe(void);
void del_str(char* _Owner _Nullable s);

/* pinned: the warning fires exactly once by object name, not once per leaf */
void consumed_object_reports_once(void)
{
    struct wide* _Owner p = make();
    if (p == NULL)
        return;

    if (maybe())
        goto cleanup;

    p = tail(p);

    if (p == NULL)
        goto cleanup;

cleanup:
    del(p); //lint 31 one report for the object, not one per member
}

/* control: moving a single owner member out still names that member, reading the others is fine */
void partial_move_names_the_member(struct inner* _Nullable in)
{
    if (in == NULL)
        return;

    char* _Owner taken = in->s; /* only .s moves; .i is untouched */
    in->s = NULL;

    if (in->i > 0)
        in->i = 0;

    del_str(taken);
}
