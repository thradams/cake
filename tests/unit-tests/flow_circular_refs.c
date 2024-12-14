#pragma safety enable


struct item
{
    struct item* _Opt other;
};

struct items
{
    struct item * _Opt one;
    struct item * _Opt two;
};

void f(struct items i){}

int main()
{
    struct item one = {0};
    struct item two = {0};
    struct items items = {0};
    items.one = &two;
    items.two = &one;
    one.other = &two;
    two.other = &one;
    f(items);
}