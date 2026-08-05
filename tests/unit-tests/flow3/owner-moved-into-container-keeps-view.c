#pragma safety enable

/*
   Moving an owner into a container while keeping a non-owner pointer to the
   moved item.

   This is the standard shape for "insert, then keep working with what you just
   inserted": the container takes ownership, and the caller retains a plain
   (non-owner) pointer to reach the item afterwards. The container outlives the
   call, so the view stays valid.

       struct item* _Owner it = make_item();
       struct item* view = it;      // non-owner alias
       list_add(&list, it);         // ownership moves into the container
       use(view);                   // still valid: the list owns it now

   It is the same family as self-consuming-reassignment.c (`p = f(p)`): an owner
   is consumed by a call, and the question is what the analysis believes about
   the names still referring to it afterwards.
*/

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

/* The other way to keep a usable pointer: the container hands back a
   non-owner pointer to what it just took ownership of. */
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

    /*
       KNOWN GAP. Valid code: the list owns the item now and outlives this call,
       so `view` still points at live storage.

       list_add consumes the _Owner argument, which ends the lifetime of the
       POINTEE -- and `view` aliases that same pointee, so the ENDED state
       reaches it. flow3 has no way to know the container keeps the object
       alive. Marking `view` _View does not help (tried): _View governs whether
       the object's own _Owner members are treated as owned, not whether an
       alias survives a move of what it points at.

       The two functions below are the same scenario without an alias -- reading
       the item back out of the container, and a build loop -- and both are
       already clean, which isolates the gap to the retained alias.
    */
    use(view); //lint 31 object '(*view)' lifetime has ended -- alias of an owner moved into a container
}

/* The container returns the item it just took: no alias is retained, so there
   is nothing for the move to invalidate. This is the form to prefer. */
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

/* A build loop: each round creates an item and hands it to the container. The
   owner variable is reassigned every iteration, so nothing accumulates. */
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
