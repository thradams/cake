#pragma safety enable
#include <assert.h>
#include <stddef.h>

/*
    The destination of an assignment can be reached through its source:
    with child == parent, `parent->last_child` is a member of what `child`
    points to. The source is read before the destination is written, so the
    read must not find the value being stored (here a copy of the already
    moved `child`, which reported "'child->last_child' was moved").
*/

struct node
{
    struct node* _Owner first_child;
    struct node* last_child;
    struct node* _Owner next;
};

void append_child(struct node* parent, struct node* _Owner child)
{
    if (parent->first_child == NULL)
    {
        parent->first_child = child;
    }
    else
    {
        assert(parent->last_child &&
               parent->last_child->next == NULL);
        parent->last_child->next = child; //lint 26 33
    }
    parent->last_child = child;
}
