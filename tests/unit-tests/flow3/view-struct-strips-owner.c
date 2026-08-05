#pragma safety enable

struct node { int x; };

struct holder {
    struct node* _Owner p;
    struct node* _Owner q;
};

void f(struct node* view_a, struct node* view_b)
{
    _View struct holder x = { 0 };
    x.p = view_a;
    x.q = view_b;
}
