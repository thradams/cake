#pragma safety enable

/* a plain pointer parameter is a borrow: every _Owner member it frees must be restored on every exit; only _Dtor may consume */

typedef unsigned long size_t;
void* _Owner _Opt _Uninitialized malloc(size_t sz);
void free(void* _Owner _Opt p);
char* _Owner _Opt strdup(const char* s);

struct person {
    char* _Opt _Owner name;
};

/* Must warn: early return leaves p->name freed and never reassigned. */
void set_leaves_dangling_on_early_return(struct person* p, char* name)
{
    free(p->name);
    char* _Opt _Owner temp = strdup(name);
    if (temp == 0) return; /* warns: p->name moved/released here, never restored */ //lint 72
    p->name = temp;
}

/* Must NOT warn: the failure path restores p->name before returning. */
void set_restores_on_failure(struct person* p, char* name)
{
    free(p->name);
    p->name = 0;
    char* _Opt _Owner temp = strdup(name);
    if (temp == 0) return;
    p->name = temp;
}

/* must NOT warn: p->name freed and reassigned unconditionally */
void set_always_reassigns(struct person* p, char* name)
{
    free(p->name);
    p->name = strdup(name);
}

/* must NOT warn: a _Dtor parameter may consume the pointee's owner members */
void person_destroy(_Dtor struct person* p)
{
    free(p->name);
}

/* must NOT warn: p itself is _Owner, covered by the "owner not moved" check instead */
void person_take_and_destroy(struct person* _Owner _Opt p)
{
    if (p)
    {
        free(p->name);
        free(p);
    }
}
