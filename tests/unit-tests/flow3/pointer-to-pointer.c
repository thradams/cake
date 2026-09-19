#pragma safety enable

/* T** nullability: checking pp then *pp narrows (*pp)->member; `*pp = get(); static_debug(*pp)` used to crash on a NULL REF target; two documented gaps: unchecked *pp does not warn, and narrowing after `*pp = get()` does not reach `(*pp)->` */

struct node
{
    struct node* _Opt next;
    int value;
};

struct node* _Opt get();

int both_levels_checked(struct node* _Opt* _Opt pp)
{
    if (pp)
    {
        if (*pp)
        {
            /* safe -- both pp and *pp narrowed non-null */
            return (*pp)->value;
        }
    }
    return -1;
}

int pp_unchecked(struct node* _Opt* _Opt pp)
{
    /* pp itself was never checked */
    return (*pp)->value; //lint 33 possible null pointer dereference
}

int chain_both_links_checked(struct node* _Opt p)
{
    if (p && p->next)
    {
        /* safe -- p->next narrowed non-null */
        return p->next->value;
    }
    return -1;
}

void assign_through_pp_no_crash(struct node* _Opt* _Opt pp)
{
    if (pp)
    {
        *pp = get();
        /* expected: *pp == null or *pp == <non-null object>; previously crashed here */
        // static_debug(*pp);
    }
}

int pp_checked_star_pp_not_checked(struct node* _Opt* _Opt pp)
{
    if (pp)
    {
        /* known gap: *pp is never checked here but flow3 does not warn */
        return (*pp)->value;
    }
    return -1;
}

int chain_only_first_link_checked(struct node* _Opt p)
{
    if (p)
    {
        /* known gap: p->next is not checked, same class as above */
        return p->next->value; //lint 33 -> operator applied to a possible null pointer 'p->next'
    }
    return -1;
}

int assign_then_check_still_warns(struct node* _Opt* _Opt pp)
{
    if (pp)
    {
        *pp = get();
        if (*pp)
        {
            /* known gap: *pp was just narrowed non-null by the if above but the narrowing does not reach this access */
            return (*pp)->value; //lint 33 -> operator applied to a null pointer
        }
    }
    return -1;
}
