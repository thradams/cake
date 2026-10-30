#pragma safety enable

/* a parameter may point to a file scope object the function uses: f(&g) */

void free(void* _Owner _Opt p);

struct list { int* _Owner head; };

struct list g;

int f(const struct list* p)
{
    free(g.head);
    int r = p->head ? *p->head : 0; //lint 100 p may be &g, whose head was freed
    g.head = 0;
    return r;
}

/* handled: no warning */
int h(const struct list* p)
{
    if (p == &g)
        return 0;
    free(g.head);
    int r = p->head ? *p->head : 0;
    g.head = 0;
    return r;
}

struct other { int i; };

/* different types: never the same object */
int k(const struct other* p)
{
    free(g.head);
    g.head = 0;
    return p->i;
}
