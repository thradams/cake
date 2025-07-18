struct list {
    struct item * head;
    struct item * tail;
};

struct item {
    struct item * next;
};


static void _cake_zmem(void *dest, register unsigned int len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0) *ptr++ = 0;
}

void item_delete(struct item * p);

void list_destroy(struct list * list)
{
    struct item * p;

    p = list->head;
    while (p)
    {
        struct item * next;

        next = p->next;
        p->next = 0;
        item_delete(p);
        p = next;
    }
}

int main()
{
    struct list  list;

    _cake_zmem(&list, 8);
    list_destroy(&list);
}

