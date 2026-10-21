/* warning 82: the non-const twin of a const lookup returns (T*) f_const(p), so p is not suggested const */

struct item { int value; struct item * next; };
struct list { struct item * head; };

const struct item * find_const(const struct list * list)
{
    return list->head;
}

struct item * find(struct list * list)
{
    return (struct item *) find_const(list);
}

int count_first(struct list * list)
{
    const struct item * p = find_const(list);
    return p ? p->value : 0;
} //lint 82
