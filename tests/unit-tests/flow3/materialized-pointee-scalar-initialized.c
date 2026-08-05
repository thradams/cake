#pragma safety enable

/*
   Regression test: a struct fully copied from a pointer's pointee is treated as
   initialized when the pointee's members are themselves known.

   flow3 must NOT report the copy's scalar members as "possible uninitialized
   object" when the source is a non-optional pointer (whose pointee is modeled
   with concrete member state). This pins the false-positive-free case.

   NOTE (investigation, 2026-07): the sibling case where the source pointer is
   `_Opt` and only narrowed non-null before the copy (e.g. type.c
   `struct type tmp = md->declarator->type;`) can still be reported as
   "possible uninitialized" because the narrowed pointee's scalar members are
   left with no flow3 state ("empty"). That is a separate, still-open item; the
   attempted fix (seeding a materialized pointee's integer members ANY) was
   reverted for lacking a clean, minimal reproduction. This sample deliberately
   covers only the currently-clean non-_Opt case.

   This sample must be clean.
*/

struct inner {
    unsigned long long n;
    int flags;
};

struct outer {
    struct inner in;
};

struct holder {
    struct outer* p;   /* non-optional: pointee has concrete member state */
};

void take_inner(const struct inner* q);

void copy_is_initialized(struct holder* h)
{
    struct inner tmp = h->p->in;   /* struct copy: initializes every member */
    tmp.flags = 0;
    take_inner(&tmp);              /* must NOT warn "possible uninitialized" */
}
