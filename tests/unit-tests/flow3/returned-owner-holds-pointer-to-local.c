#pragma safety enable

/* issue #512: the returned heap object holds the address of a local that dies at return */

void* _Owner _Opt malloc(unsigned long n);

struct V
{
    int* q;
};

struct V* _Owner _Opt f(void)
{
    struct V* _Owner _Opt v = malloc(sizeof(struct V));
    if (!v) return 0;
    int x = 1;
    v->q = &x;
    return v; //lint 31 returned 'v->q' points to an object whose lifetime has ended
}
