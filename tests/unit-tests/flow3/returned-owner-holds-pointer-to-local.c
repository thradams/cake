#pragma safety enable

/* issue #512: the returned heap object holds the address of a local that dies at return */

void* _Owner malloc(unsigned long n);

struct V
{
    int* _Nonnull q;
};

struct V* _Owner f(void)
{
    struct V* _Owner v = malloc(sizeof(struct V));
    if (!v) return 0;
    int x = 1;
    v->q = &x;
    return v; //lint 31 returned 'v->q' points to an object whose lifetime has ended
}
