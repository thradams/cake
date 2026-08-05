#pragma safety enable

/*
   A plain pointer parameter (not _Dtor, not _Ctor, not itself _Owner) is a
   BORROW: the callee may read and write through it, but every _Owner
   member it touches must still be a real, live value at every exit --
   exactly as if it were a local owner going out of scope. Only a _Dtor
   parameter is allowed to consume (move/free) the pointee's owner members
   without restoring them.

   Motivating bug report:

       void set(struct person* p, char* name) {
           free(p->name);
           char* _Opt _Owner temp = strdup(name);
           if (temp == NULL) return;   // p->name freed but never restored!
           p->name = temp;
       }

   If strdup(name) returns NULL, set() returns early without ever assigning
   to p->name -- but p->name was already freed a few lines above. The
   caller is left holding a struct whose _Owner member is a dangling
   pointer to freed memory. Before this check existed, nothing caught this:
   p is neither _Ctor (whose contract is "must end up initialized") nor
   _Dtor (whose contract is "must end up fully released").
*/

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

/* Must NOT warn: p->name is freed and reassigned unconditionally, no early
   exit in between. */
void set_always_reassigns(struct person* p, char* name)
{
    free(p->name);
    p->name = strdup(name);
}

/* Must NOT warn: a _Dtor parameter is explicitly allowed to consume its
   pointee's owner members and never give them back -- that is the whole
   point of _Dtor. */
void person_destroy(_Dtor struct person* p)
{
    free(p->name);
}

/* Must NOT warn: p itself is _Owner, so this function is erasing/consuming
   the whole object it was handed, not borrowing someone else's -- already
   covered by the existing "owner not moved" mechanism at the point of
   erasure, not this check. */
void person_take_and_destroy(struct person* _Owner _Opt p)
{
    if (p)
    {
        free(p->name);
        free(p);
    }
}
