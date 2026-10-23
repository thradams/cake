#pragma safety enable

/* two const views only read: they are not analysed as the same object */

struct X { int i; };

int same(const struct X* a, const struct X* b)
{
    return a->i == b->i;
}

/* one writes: the pair is analysed */
void write_then_read(struct X* a, const struct X* b)
{
    a->i = 1;
    int v = b->i;
    (void)v;
}
