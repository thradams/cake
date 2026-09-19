#pragma safety enable

/* moving an owner into a container while keeping a non-owner pointer to the item: the container outlives the call, the view stays valid */

#define NULL ((void*)0)

struct item
{
    int value;
    struct item* _Owner _Opt next;
};

struct list
{
    struct item* _Owner _Opt head;
};

struct item* _Owner _Opt make_item(void);
void list_add(struct list* l, struct item* _Owner it);

/* the container hands back a non-owner pointer to what it took */
struct item* list_add_r(struct list* l, struct item* _Owner it);
void list_destroy(_Dtor struct list* l);
void use(const struct item* it);

/* The plain form: move in, then use the retained view. */
void add_then_use_view(struct list* l)
{
    struct item* _Owner _Opt it = make_item();
    if (it == NULL)
        return;

    const struct item* view = it; /* non-owner alias, taken before the move */

    list_add(l, it);

    /* KNOWN GAP: list_add ends the pointee's lifetime and `view` aliases it, flow3 cannot know the container keeps it alive; _View on the alias does not help */
    use(view); //lint 31 object '(*view)' lifetime has ended -- alias of an owner moved into a container
}

/* the container returns the item, no alias retained: the form to prefer */
void add_returning_item(struct list* l)
{
    struct item* _Owner _Opt it = make_item();
    if (it == NULL)
        return;

    struct item* inserted = list_add_r(l, it);

    use(inserted);
}

/* The same, reaching the item back out of the container instead of aliasing. */
void add_then_use_head(struct list* l)
{
    struct item* _Owner _Opt it = make_item();
    if (it == NULL)
        return;

    list_add(l, it);

    if (l->head)
        use(l->head);
}

/* build loop: the owner variable is reassigned each round, nothing accumulates */
void build(struct list* l, int n)
{
    while (n-- > 0)
    {
        struct item* _Owner _Opt it = make_item();
        if (it == NULL)
            return;

        list_add(l, it);
    }
}
