/* a pointer reached through const copied or returned as a non-const pointer lets the caller change what const promised */

struct node { int i; };
struct list { struct node* _Nonnull head; };

void copy_then_write(const struct list* list)
{
    struct node* p = list->head; //lint 120
    p->i = 1;
}

void assign_then_write(const struct list* list)
{
    struct node* p;
    p = list->head; //lint 120
    p->i = 1;
}

struct node* _Nonnull get(const struct list* list)
{
    return list->head; //lint 120
}

const struct node* _Nonnull get_const(const struct list* list)
{
    return list->head;
}

int read_through_const_copy(const struct list* list)
{
    const struct node* p = list->head;
    return p->i;
}

struct node* _Nonnull get_cast(const struct list* list)
{
    return (struct node*)list->head;
}
