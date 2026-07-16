/* Cake 0.14.09 x86_msvc 2026-07-15 23:41:28 */

struct item;

struct items {
    struct item * one;
    struct item * two;
};

struct item {
    struct item * other;
};

static void * memset(void *dest, int ch, unsigned int count);

void f(struct items i)
{
}

int main()
{
    struct item one;
    struct item two;
    struct items items;

    memset(&one, 0, 4);
    memset(&two, 0, 4);
    memset(&items, 0, 8);
    items.one = &two;
    items.two = &one;
    one.other = &two;
    two.other = &one;
    f(items);
}

static void * memset(void *ptr, int value, unsigned int count)
{
    unsigned char *p;
    unsigned char v;

    p = (unsigned char *) ptr;
    v = (unsigned char) value;
    while (count--)
    {
        *p++ = v;
    }

    return ptr;
}
