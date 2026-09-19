#pragma safety enable

/* `_Opt struct X* p` qualifies the pointee: p is non-null but p->p_not_opt is possibly null; through a plain pointee it stays non-null */

struct X
{
    char* p_not_opt; /* not _Opt -- non-null by its own declared type */
};

void through_opt_pointee(_Opt struct X* p)
{
    /* p can't be null -- no warning here even without a guard. */
    *p->p_not_opt = 'a'; //lint 33 possible null pointer dereference
}

void through_plain(struct X* p)
{
    *p->p_not_opt = 'a'; // ok: pointee has no _Opt taint, member stays non-null by contract
}

/* `_Dtor _Opt struct X* p` and `_Dtor struct X* p` are equivalent: a _Dtor pointee is already permissive */
void destroy_x1(_Dtor _Opt struct X* p) { (void)p; }
void destroy_x2(_Dtor struct X* p) { (void)p; }

/* _View taints members the same way for ownership: every _Owner member of a _View object is a view */
