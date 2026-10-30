#pragma safety enable

/* a struct copied from a non-optional pointer's pointee is initialized; the narrowed `_Nullable` source case is a separate open item */

struct inner {
    unsigned long long n;
    int flags;
};

struct outer {
    struct inner in;
};

struct holder {
    struct outer* _Nonnull p;   /* non-optional: pointee has concrete member state */
};

void take_inner(const struct inner* q);

void copy_is_initialized(const struct holder* h)
{
    struct inner tmp = h->p->in;   /* struct copy: initializes every member */
    tmp.flags = 0;
    take_inner(&tmp);              /* must NOT warn "possible uninitialized" */
}
