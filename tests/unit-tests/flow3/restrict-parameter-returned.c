#pragma safety enable

struct item { int value; struct item* _Nonnull next; };

int* _Nonnull same(int* restrict p)
{
    return p; //lint 102
}

int* _Nonnull inside(struct item* restrict it)
{
    return &it->value; //lint 102
}

int* _Nonnull through_copy(int* restrict p)
{
    int* q = p;
    return q; //lint 102
}

struct item* _Nonnull next_of(struct item* restrict it)
{
    return it->next; /* another object */
}

int* _Nonnull maybe(int* restrict p, int* other, int c)
{
    if (c)
    {
        return other;
    }
    return p; //lint 102
}
