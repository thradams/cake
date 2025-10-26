/* Cake 0.12.26 x86_msvc */
struct item;

struct items {
    struct item * one;
    struct item * two;
};

struct item {
    struct item * other;
};


static void _cake_zmem(void *dest, unsigned int len)
{
  unsigned char *ptr;

  ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void f(struct items  i)
{
}


int main()
{
    struct item  one;
    struct item  two;
    struct items  items;

    _cake_zmem(&one, 4);
    _cake_zmem(&two, 4);
    _cake_zmem(&items, 8);
    items.one = &two;
    items.two = &one;
    one.other = &two;
    two.other = &one;
    f(items);
}


