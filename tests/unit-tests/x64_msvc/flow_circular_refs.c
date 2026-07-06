/* Cake x64_msvc */

struct item;

struct items {
    struct item * one;
    struct item * two;
};

struct item {
    struct item * other;
};

static void memset(void *dest, int ch, unsigned long long count);

void f(struct items i)
{
}

int main()
{
    struct item one;
    struct item two;
    struct items items;

    memset(&one, 0, 8);
    memset(&two, 0, 8);
    memset(&items, 0, 16);
    items.one = &two;
    items.two = &one;
    one.other = &two;
    two.other = &one;
    f(items);
}

static void memset(void *dest, int ch, unsigned long long count)
{
    unsigned char *ptr;

    ptr = (unsigned char*)dest;
    while (count-- > 0)
    {
       *ptr++ = 0;
    }
}

