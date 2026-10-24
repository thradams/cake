#pragma safety enable

struct item { int value; struct item* next; };

int* same(int* restrict p)
{
    return p; //lint 102
}

int* inside(struct item* restrict it)
{
    return &it->value; //lint 102
}

int* through_copy(int* restrict p)
{
    int* q = p;
    return q; //lint 102
}

struct item* next_of(struct item* restrict it)
{
    return it->next; /* another object */
}

int* maybe(int* restrict p, int* other, int c)
{
    if (c)
    {
        return other;
    }
    return p; //lint 102
}
