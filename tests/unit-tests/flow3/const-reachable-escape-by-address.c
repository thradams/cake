/* taking the address of something reached through const lets it out like copying the pointer */

struct item { int value; };
struct node { struct item* _Nonnull item; };

void set(int* p);

void copy_address(const struct node* node)
{
    int* p = &node->item->value; //lint 120
    *p = 1;
}

void pass_address(const struct node* node)
{
    set(&node->item->value); //lint 120
}

const int* _Nonnull read_address(const struct node* node)
{
    return &node->item->value;
}