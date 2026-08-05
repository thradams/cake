#pragma safety enable

struct node { int x; };
struct holder { struct node* _Owner p; };

void take(struct node* _Owner n);

void f(struct node* _Owner real)
{
    _View struct holder x = { 0 };
    x.p = real;
    take(real);
}
