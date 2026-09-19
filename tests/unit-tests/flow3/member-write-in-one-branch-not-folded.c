/* no #pragma safety enable: members reached through a pointer are not seeded */

struct arena
{
    int size;
    int capacity;
};

void one_arm_writes_member(struct arena* a, int c)
{
    if (c)
    {
        a->size = 0;
    }

    if (a->size == 5)
    {
        a->capacity = 1; /* reachable, size is 0 or unknown */
    }
}

void grow_when_full(struct arena* a, int is_new)
{
    if (is_new)
    {
        a->size = 0;
        a->capacity = 10;
    }

    if (a->size == a->capacity)
    {
        a->capacity = a->capacity == 0 ? 4 : a->capacity * 2; /* reachable */
    }
}

void both_arms_write_member(struct arena* a, int c)
{
    if (c)
    {
        a->size = 0;
    }
    else
    {
        a->size = 0;
    }

    if (a->size == 5)
    {
        a->capacity = 1; //lint 68 /* unreachable, size is 0 on both arms */
    }
}
